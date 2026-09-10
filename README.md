# imgui-demo

A small declarative language for Dear ImGui interfaces, and the parser that runs
it.

Layout is written in a `.ui` file rather than in C++. The parser reads it, and
`UIManager` builds the ImGui calls each frame.

```
ui {
    win "My Demo" {
        size: [400, 300]
        background: [0.2, 0.2, 0.2]
        resizable: false
        movable: false

        txt "Count: {count}" @ center(y:100)
        btn "+" @ below(20) >> count++
    }
}
```

Three things are worth pointing at. `{count}` interpolates a value out of the UI
state into the label. `@ center(y:100)` and `@ below(20)` are relative placement,
so an element positions itself against the window or against the element above
it. `>> count++` binds an action to the button without a callback in C++.

`ui_config.json` is the same interface expressed as JSON, which is what the
parser was replacing.

| File | |
|---|---|
| `ui_parser.{h,cpp}` | Tokenises and parses the `.ui` grammar. |
| `ui_manager.{h,cpp}` | Holds state and emits ImGui calls per frame. |
| `demo.ui` | The example above. |
| `test_parser.cpp` | Parser tests. |
| `main.cpp` | GLFW window plus the OpenGL 3 ImGui backend. |

## Build

Needs Dear ImGui with the GLFW and OpenGL 3 backends, and GLFW.
