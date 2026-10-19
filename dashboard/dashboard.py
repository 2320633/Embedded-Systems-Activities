#!/usr/bin/env python3
"""Generate the repository's embedded-systems dashboard README."""

from __future__ import annotations

import json
import re
from collections import defaultdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


DASHBOARD_DIR = Path(__file__).resolve().parent
ROOT_DIR = DASHBOARD_DIR.parent


def load_config() -> dict[str, Any]:
    config_path = DASHBOARD_DIR / "config.json"
    config = json.loads(config_path.read_text(encoding="utf-8"))
    required = (
        "project_name",
        "project_summary",
        "source_directory",
        "source_extensions",
        "technology_signals",
        "learning_path",
        "engineering_checklist",
        "references",
        "build_manifests",
    )
    missing = [key for key in required if key not in config]
    if missing:
        raise ValueError(f"Missing required dashboard configuration: {', '.join(missing)}")
    if not isinstance(config["source_extensions"], list) or not config["source_extensions"]:
        raise ValueError("source_extensions must be a non-empty list")
    return config


def markdown_escape(value: str) -> str:
    return value.replace("|", "\\|").replace("\r", " ").replace("\n", " ")


def bullet_list(items: list[str]) -> str:
    return "\n".join(f"- {item}" for item in items)


def get_sources(config: dict[str, Any]) -> list[Path]:
    source_root = ROOT_DIR / config["source_directory"]
    if not source_root.is_dir():
        raise FileNotFoundError(f"Configured source directory does not exist: {source_root}")
    extensions = {extension.lower() for extension in config["source_extensions"]}
    return sorted(
        (
            path
            for path in source_root.rglob("*")
            if path.is_file() and path.suffix.lower() in extensions
        ),
        key=lambda path: path.relative_to(ROOT_DIR).as_posix().lower(),
    )


def detect_signals(
    source_texts: dict[Path, str], signals: list[dict[str, str]]
) -> tuple[dict[str, list[dict[str, Any]]], dict[Path, list[str]]]:
    by_category: dict[str, list[dict[str, Any]]] = defaultdict(list)
    by_file: dict[Path, list[str]] = defaultdict(list)

    for signal in signals:
        expression = re.compile(signal["pattern"], re.IGNORECASE)
        matching_paths = [
            path for path, source in source_texts.items() if expression.search(source)
        ]
        if not matching_paths:
            continue
        item = {
            "name": signal["name"],
            "category": signal["category"],
            "paths": matching_paths,
        }
        by_category[signal["category"]].append(item)
        for path in matching_paths:
            by_file[path].append(signal["name"])

    return by_category, by_file


def make_activity_rows(
    sources: list[Path],
    source_texts: dict[Path, str],
    file_signals: dict[Path, list[str]],
    titles: dict[str, str],
) -> str:
    rows = []
    for path in sources:
        relative_path = path.relative_to(ROOT_DIR).as_posix()
        title = titles.get(path.name, path.stem.replace("_", " ").title())
        concepts = ", ".join(file_signals.get(path, [])) or "No configured signals detected"
        line_count = sum(bool(line.strip()) for line in source_texts[path].splitlines())
        rows.append(
            f"| [{markdown_escape(title)}]({relative_path}) "
            f"| {markdown_escape(concepts)} | {line_count} |"
        )
    return "\n".join(rows) if rows else "| No firmware examples found | — | 0 |"


def make_capability_sections(
    categories: dict[str, list[dict[str, Any]]]
) -> str:
    sections = []
    for category, signals in categories.items():
        items = []
        for signal in signals:
            paths = ", ".join(
                f"[{path.name}]({path.relative_to(ROOT_DIR).as_posix()})"
                for path in signal["paths"]
            )
            items.append(f"- **{markdown_escape(signal['name'])}:** {paths}")
        sections.append(f"### {markdown_escape(category)}\n\n" + "\n".join(items))
    return "\n\n".join(sections) if sections else "_No configured capabilities detected yet._"


def make_learning_path(stages: list[dict[str, str]]) -> str:
    return "\n".join(
        f"{index}. **{markdown_escape(stage['stage'])}** — "
        f"{markdown_escape(stage['focus'])}"
        for index, stage in enumerate(stages, start=1)
    )


def make_references(references: list[dict[str, str]]) -> str:
    return "\n".join(
        f"- [{markdown_escape(reference['title'])}]({reference['url']})"
        for reference in references
    )


def platform_summary(categories: dict[str, list[dict[str, Any]]]) -> str:
    platforms = [
        signal["name"]
        for signal in categories.get("Microcontrollers", [])
    ]
    return ", ".join(platforms) if platforms else "Not identified by configured source patterns"


def build_status(config: dict[str, Any]) -> str:
    manifests = [
        name for name in config["build_manifests"] if (ROOT_DIR / name).is_file()
    ]
    if manifests:
        return "Build manifest found: " + ", ".join(f"`{name}`" for name in manifests) + "."
    return "No repository-level build manifest found; sketches are inventoried as source examples."


def render_dashboard(config: dict[str, Any]) -> str:
    sources = get_sources(config)
    source_texts = {
        path: path.read_text(encoding="utf-8", errors="replace") for path in sources
    }
    categories, file_signals = detect_signals(
        source_texts, config["technology_signals"]
    )
    sensors = len(categories.get("Sensors", []))
    interfaces = len(categories.get("Hardware interfaces", []))
    total_lines = sum(
        sum(bool(line.strip()) for line in source.splitlines())
        for source in source_texts.values()
    )

    template = (DASHBOARD_DIR / "README_TEMPLATE.md").read_text(encoding="utf-8")
    values = {
        "project_summary": markdown_escape(config["project_summary"]),
        "generated_at": datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M UTC"),
        "activity_count": str(len(sources)),
        "source_lines": str(total_lines),
        "sensor_count": str(sensors),
        "interface_count": str(interfaces),
        "platform_summary": markdown_escape(platform_summary(categories)),
        "build_status": build_status(config),
        "source_directory": config["source_directory"],
        "source_extensions": ", ".join(config["source_extensions"]),
        "activity_rows": make_activity_rows(
            sources,
            source_texts,
            file_signals,
            config.get("activity_titles", {}),
        ),
        "capability_sections": make_capability_sections(categories),
        "learning_path": make_learning_path(config["learning_path"]),
        "engineering_checklist": bullet_list(config["engineering_checklist"]),
        "references": make_references(config["references"]),
    }
    rendered = template
    for name, value in values.items():
        rendered = rendered.replace("{{" + name + "}}", value)

    unresolved = re.findall(r"\{\{[^{}]+\}\}", rendered)
    if unresolved:
        raise ValueError(f"Unresolved dashboard template placeholders: {', '.join(unresolved)}")
    return rendered


def main() -> None:
    output_path = ROOT_DIR / "README.md"
    output_path.write_text(render_dashboard(load_config()), encoding="utf-8", newline="\n")
    print(f"Updated {output_path.relative_to(ROOT_DIR).as_posix()}")


if __name__ == "__main__":
    main()
