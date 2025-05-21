### Paths
# How to build
create a build directory called `build`
then type this command to create a makefile in the build directory

(you dont need to do `-G "MinGW Makefiles"` if you are not using mingw)

```bash
cmake -B build -G "MinGW Makefiles"
```

then once you created the makefile, use the makefile using make

in your commands, go to the project directory and do `./build/Paths`

then it will run

### to build web
Create a build directory called `build-web`

Go inside the folder and run these commands
```bash
emcmake cmake -G Ninja
cmake --build .
```
(not sure if `-G Ninja` is necessary)

then create an html file with this code:

```html
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8" />
</head>
<body>
    <canvas class="relative w-2/5" style="height: 50vh; overflow: hidden;" id="canvas" oncontextmenu="event.preventDefault()"></canvas>
    <script>
        var Module = {
            canvas: (function() { return document.getElementById('canvas'); })()
        };
    </script>
    <script src="Paths.js"></script>
</body>
</html>
```
