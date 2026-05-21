Excellent — PlantUML is a *fantastic* choice for text-based UML or flow diagrams, and it works perfectly on Debian.
Let’s go step-by-step: installing it, verifying it, and making your first **flowchart**.

---

## 🧩 1. Install Java (required)

PlantUML runs on Java. Most Debian systems don’t have it by default.

```bash
sudo apt update
sudo apt install default-jre
```

Verify:

```bash
java -version
```

You should see something like:

```
openjdk version "17.x.x"
```

---

## 🌱 2. Install Graphviz (for rendering arrows/shapes)

PlantUML uses **Graphviz (dot)** for layout.

```bash
sudo apt install graphviz
```

Verify:

```bash
dot -V
```

You should see:

```
dot - graphviz version 2.xx.x
```

---

## 📦 3. Install PlantUML

### Option A — from Debian repository

```bash
sudo apt install plantuml
```

That’s it!
It will install `/usr/bin/plantuml`, and you can run it directly.

### Option B — if you want latest version

```bash
sudo apt install wget
wget https://github.com/plantuml/plantuml/releases/latest/download/plantuml.jar -O ~/plantuml.jar
```

Then run it via:

```bash
java -jar ~/plantuml.jar file.puml
```

---

## 🧱 4. Create your first PlantUML file

Let’s make a simple **flow diagram**.

Create a file named `flow.puml` (or `flow.uml`):

```bash
nano flow.puml
```

Paste this example:

```plantuml
@startuml
title Sample Flow Diagram

start
:Initialize system;
if (Is input valid?) then (yes)
  :Process data;
  if (More data?) then (yes)
    :Loop back;
    repeat
  else (no)
    :Show results;
  endif
else (no)
  :Show error message;
endif
stop

@enduml
```

Save and exit (`Ctrl+O`, `Enter`, `Ctrl+X`).

---

## 🧾 5. Generate the diagram

Run:

```bash
plantuml flow.puml
```

or (if you’re using the `.jar` manually)

```bash
java -jar ~/plantuml.jar flow.puml
```

It will produce:

```
flow.png
```

Open it:

```bash
xdg-open flow.png
```

You’ll see your **flow diagram** with start/end nodes, conditionals, and actions.

---

## 💡 6. Optional – VS Code / Editor integration

If you use Visual Studio Code:

1. Install extension **“PlantUML”** by jebbs.
2. Set your PlantUML jar path in settings if needed.
3. You can render live previews with `Alt+D`.

---

## 🧠 7. Quick summary of diagram types you can do

PlantUML supports:

* Flowcharts (like above)
* UML class diagrams
* Sequence diagrams
* Activity/state diagrams
* Use-case diagrams
* Entity-relationship (ER)
* Gantt charts, mind maps, wireframes, etc.

---

✅ **In short:**

```bash
sudo apt install default-jre graphviz plantuml
plantuml mydiagram.puml
```

And that’s it — you’re producing diagrams from plain text!

---

Would you like me to show you a **more complex flowchart example** (with subprocesses, parallel branches, and notes)?
