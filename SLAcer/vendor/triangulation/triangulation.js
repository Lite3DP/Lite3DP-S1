'use strict';

if ( ! window.THREE ) throw new Error( 'ERROR: three.js not loaded' );

THREE.Triangulation = ( function() {

    var timer = false;

    var library = 'original';

    /**
     * Names of the supported libraries
     *
     * @type {{original: string, earcut: string, poly2tri: string, libtess: string}}
     */
    var libraries = {
        original: 'original',
        earcut: 'earcut',
        poly2tri: 'poly2tri',
        libtess: 'libtess'
    };

    /**
     * Container object for holding the different library adapters
     *
     * @type {{}}
     */
    var adapters = {};

    adapters[ libraries.original ] = {

        triangulate: THREE.ShapeUtils.triangulate,

        triangulateShape: THREE.ShapeUtils.triangulateShape

    };

    adapters[ libraries.earcut ] = {

        triangulateShape: function( contour, holes ) {

            var i, il, dim = 2, array;
            var holeIndices = [];
            var points = [];

            addPoints( contour );

            for ( i = 0, il = holes.length; i < il; i ++ ) {

                holeIndices.push( points.length / dim );

                addPoints( holes[ i ] );

            }

            array = earcut( points, holeIndices, dim );

            var result = [];

            for ( i = 0, il = array.length; i < il; i += 3 ) {

                result.push( array.slice( i, i + 3 ) );

            }

            return result;

            function addPoints( a ) {

                var i, il = a.length;

                for ( i = 0; i < il; i ++ ) {

                    points.push( a[ i ].x, a[ i ].y );

                }

            }

        }

    };

    adapters[ libraries.poly2tri ] = {

        triangulateShape: function( contour, holes ) {

            var i, il, object, sweepContext, triangles;
            var pointMap = {}, count = 0;

            points = makePoints( contour );

            sweepContext = new poly2tri.SweepContext( points );

            for ( i = 0, il = holes.length; i < il; i ++ ) {

                points = makePoints( holes[ i ] );

                sweepContext.addHole( points );

                points = points.concat( points );

            }

            object = sweepContext.triangulate();

            triangles = object.triangles_;

            var a, b, c, points, result = [];

            for ( i = 0, il = triangles.length; i < il; i ++ ) {

                points = triangles[ i ].points_;

                a = pointMap[ points[ 0 ].x + ',' + points[ 0 ].y ];
                b = pointMap[ points[ 1 ].x + ',' + points[ 1 ].y ];
                c = pointMap[ points[ 2 ].x + ',' + points[ 2 ].y ];

                result.push( [ a, b, c ] );

            }

            return result;

            function makePoints( a ) {

                var i, il = a.length,
                    points = [];

                for ( i = 0; i < il; i ++ ) {

                    points.push( new poly2tri.Point( a[ i ].x, a[ i ].y ) );
                    pointMap[ a[ i ].x + ',' + a[ i ].y ] = count;
                    count ++;

                }

                return points;

            }

        }

    };

    adapters[ libraries.libtess ] = {

        triangulateShape: function( contour, holes ) {

            var i, il, triangles = [];
            var pointMap = {}, count = 0;

            // libtess will take 3d verts and flatten to a plane for tesselation
            // since only doing 2d tesselation here, provide z=1 normal to skip
            // iterating over verts only to get the same answer.
            // comment out to test normal-generation code
            tessy.gluTessNormal( 0, 0, 1 );

            tessy.gluTessBeginPolygon( triangles );

            points = makePoints( contour );

            for ( i = 0, il = holes.length; i < il; i ++ ) {

                points = makePoints( holes[ i ] );

            }

            tessy.gluTessEndPolygon();

            var a, b, c, points, result = [];

            for ( i = 0, il = triangles.length; i < il; i += 6 ) {

                a = pointMap[ triangles[ i ] + ',' + triangles[ i + 1 ]];
                b = pointMap[ triangles[ i + 2 ] + ',' + triangles[ i + 3 ]];
                c = pointMap[ triangles[ i + 4 ] + ',' + triangles[ i + 5 ]];

                result.push( [ a, b, c ] );

            }

            return result;

            function makePoints( a ) {

                var i, il = a.length,
                    coordinates;

                tessy.gluTessBeginContour();

                for ( i = 0; i < il; i ++ ) {

                    coordinates = [ a[ i ].x, a[ i ].y, 0 ];
                    tessy.gluTessVertex( coordinates, coordinates );
                    pointMap[ a[ i ].x + ',' + a[ i ].y ] = count;
                    count ++;

                }

                tessy.gluTessEndContour();

                return points;

            }

        }

    };

    /**
     * Initialize the library by attaching the triangulation methods to the three.js API
     */
    function init() {

        checkDependencies( library );

        if ( timer ) {

            THREE.ShapeUtils.triangulate = function() {

                return adapters[ library ].triangulate.apply( this, arguments );

            };

            THREE.ShapeUtils.triangulateShape = function() {

                console.time( library );

                var result = adapters[ library ].triangulateShape.apply( this, arguments );

                console.timeEnd( library );

                return result;

            };

        } else {

            THREE.ShapeUtils.triangulate = adapters[ library ].triangulate;

            THREE.ShapeUtils.triangulateShape = adapters[ library ].triangulateShape;

        }

    }

    /**
     * Checks dependencies needed for the current library
     *
     * @param library
     */
    function checkDependencies( library ) {

        switch ( library ) {

            case libraries.earcut:

                if ( ! window.earcut ) throw new Error( 'ERROR: earcut not loaded' );

                break;

            case libraries.poly2tri:

                if ( ! window.poly2tri ) throw new Error( 'ERROR: poly2tri not loaded' );

                break;

            case libraries.libtess:

                if ( ! window.tessy ) throw new Error( 'ERROR: libtess not loaded' );

                break;

        }

    }

    /**
     * Set the current triangulation library
     *
     * @param name
     */
    function setLibrary( name ) {

        if ( ! libraries.hasOwnProperty( name ) ) throw new Error( 'ERROR: unknown library ' + name );

        library = name;

        init();

    }

    /**
     * Set timer for triangulation on/off
     *
     * @param boolean
     */
    function setTimer( boolean ) {

        timer = boolean;

        init();

    }

    init();

    return {

        libraries: libraries,

        setTimer: setTimer,

        setLibrary: setLibrary

    };

} )();
