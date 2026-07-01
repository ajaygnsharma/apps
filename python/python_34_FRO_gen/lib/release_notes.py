from pathlib import Path
import re
import shutil


def update_release_notes_conf(model: str, rev: str, family: str) -> Path:
  """
  Locate <MODEL>-Release_Notes/docs/conf.py (or -Release_notes),
  update release, project, and DocDescription, and return docs dir.
  # assume we already have: model = "23820"
  """
  # Be robust to underscore vs lowercase 'notes'
  candidates = [
      Path(f"{model}") / "Release_Notes" / "docs" / "conf.py",
      Path(f"{model}") / "Release_notes" / "docs" / "conf.py",
  ]
  conf_path = next((p for p in candidates if p.exists()), None)
  if conf_path is None:
      raise FileNotFoundError(
          f"Could not find conf.py in any of: {', '.join(str(p) for p in candidates)}"
      )

  text = conf_path.read_text(encoding="utf-8")

  # 1) release = "<REV>"
  text, n1 = re.subn(
      r'(^\s*release\s*=\s*)"(.*?)"(\s*)$',
      rf'\1"{rev}"\3',
      text,
      flags=re.MULTILINE,
  )

  # 2) project = "Release Notes for <FAMILY>"
  text, n2 = re.subn(
      r'(^\s*project\s*=\s*)"(.*?)"(\s*)$',
      rf'\1"Release Notes for {family}"\3',
      text,
      flags=re.MULTILINE,
  )

  # 3) \newcommand{\DocDescription}{...}  (inside the raw LaTeX preamble)
  #    Replace ANY existing content with "Release Notes for <FAMILY>"
  text, n3 = re.subn(
      r'(\\newcommand\{\\DocDescription\}\{)(.*?)(\})',
      rf'\1Release Notes for {family}\3',
      text,
      flags=re.DOTALL,
  )

  # Backup + write
  backup = conf_path.with_suffix(".py.bak")
  if not backup.exists():
      shutil.copy2(conf_path, backup)
  conf_path.write_text(text, encoding="utf-8")

  print(
      f"✅ Updated conf.py at {conf_path} "
      f"(release:{n1>0}, project:{n2>0}, DocDescription:{n3>0})"
  )
  return conf_path.parent  # docs dir

