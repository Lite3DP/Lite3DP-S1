three.js - triangulation
========

### Triangulation support ###

The aim of the project is to add support for different triangulation/tessellation algorithms/libraries to **three.js**.
Right now the library seamlessly supports the following triangulation libraries:

— [original](https://github.com/mrdoob/three.js/blob/master/src/extras/ShapeUtils.js) - original algorithm from `THREE.ShapeUtils`<br>
— [earcut](https://github.com/mapbox/earcut) - earcut triangulation library<br>
— [poly2tri](https://github.com/r3mi/poly2tri.js) - poly2tri triangulation library<br>
— [libtess](https://github.com/brendankenny/libtess.js/) - libtess tessellation library<br> 

To be able to use the **libtess** library you will have to also load the file [tessy.js](tessy.js) into your project. 

### Triangulation comparison ###

The second aim of the project is to create compare the results of these different triangulation/tessellation algorithms/libraries in **three.js**.

### Adapter ###

The library comes with an adapter [triangulation.js](triangulation.js) which can make it easy to switch between the different triangulation libraries inside your project. The implementation is simple.
The adapter is available through `THREE.Triangulation` and has two public methods:

1. `setLibrary` - sets the library that you currently want to use.
  * The available libraries are in an object called `libraries`. So to set earcut as your library: `THREE.Triangulation.setLibrary( THREE.Triangulation.libraries.earcut );` (or  simply use the string `'earcut'`).

2. `setTimer` - sets the timer on/off.
  * The timer will output the time that was needed to perform the triangulation operations inside the console giving you valuable feedback on the performance of the chosen library.
  To turn the timer on simply call: `THREE.Triangulation.setTimer( true );`


### Viewers ###

For now there are two viewers.

- One [single screen viewer](https://rawgit.com/Wilt/three.js_triangulation/master/viewers/single.html) where the triangulation result of a certain shape with a selected algorithm can be viewed.
- And [multiple screen viewer](https://rawgit.com/Wilt/three.js_triangulation/master/viewers/multiple.html) where the triangulation result of a certain shape can be seen for all algorithms at the same time.


### Benchmarking ###

The next step will be to do some benchmarking of the different algorithms

### Support for another (or your own) triangulation library ###

Do you want support for another (or your own) triangulation library, then please make an issue or a pull-request where you explain what the added value of the 
proposed library is compared to the ones that are already implemented. It would also be good to make a live example where you show that your code is working. 
This will increase the chances of the library being implemented.