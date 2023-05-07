# Mustache for Janet
Mustache module for janet via [mustach](https://gitlab.com/jobol/mustach) and [cJSON](https://github.com/DaveGamble/cJSON), based on [pymustach](https://github.com/RekGRpth/pymustach) mustach bindings.

# Usage
```janet
(import mustache)

(mustache/render "Hello, {{name}}!" "{\"name\": \"world\"}")
# --> "Hello, world!"

(mustache/render-file "Hello, {{name}}!" "{\"name\": \"world\"}" "hi.txt")
# --> nil, writes 'Hello, world!' to file 'hi.txt'
```
