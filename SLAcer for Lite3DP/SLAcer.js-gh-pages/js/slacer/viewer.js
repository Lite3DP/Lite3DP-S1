// namespace
var SLAcer = SLAcer || {};

;(function() {

    // global settings
    var globalSettings = {
        size: {
            width: 600,
            height: 400
        },
        color: 0x000000,
        antialias: true,
        target: document.body,
        camera: {
            fov: 45,
            near: 0.1,
            far: 10000,
        }
    };

    // -------------------------------------------------------------------------

    // Constructor
    function Viewer(settings) {
        // self alias
        var self = this;

        // settings settings
        self.settings = _.defaultsDeep({}, settings || {}, Viewer.globalSettings);

        // create main objects
        self.scene    = new THREE.Scene();
        self.camera   = new THREE.PerspectiveCamera();
        self.renderer = new THREE.WebGLRenderer({ antialias: self.settings.antialias });

        // assign camera settings
        _.assign(self.camera, self.settings.camera);

        // set camera orbit around Z axis
        self.camera.up = new THREE.Vector3(0, 0, 1);

        // set camera position
        self.camera.position.z = 1000;

        // set default parameters
        self.setSize(self.settings.size);
        self.setColor(self.settings.color);

        // set the target for canvas
        self.target = self.settings.target;
        self.canvas = self.renderer.domElement;
        if (self.target) {
            while (self.target.firstChild) {
                self.target.removeChild(self.target.firstChild);
            }
            self.target.appendChild(self.canvas);
        }

        // render
        self.render();
    }

    // -------------------------------------------------------------------------

    Viewer.prototype.getSize = function() {
        return this.renderer.getSize();
    };

    Viewer.prototype.setSize = function(size) {
        _.defaults(size, this.getSize());
        this.renderer.setSize(size.width, size.height);
        this.camera.aspect = size.width / size.height;
        this.camera.updateProjectionMatrix();
        return size;
    };

    Viewer.prototype.setWidth = function(width) {
        return this.setSize({ width: width });
    };

    Viewer.prototype.setHeight = function(height) {
        return this.setSize({ height: height });
    };

    // -------------------------------------------------------------------------

    Viewer.prototype.getColor = function() {
        return this.renderer.getClearColor();
    };

    Viewer.prototype.setColor = function(color) {
        this.renderer.setClearColor(color);
    };

    // -------------------------------------------------------------------------

    Viewer.prototype.removeObject = function(object) {
        object.geometry && object.geometry.dispose();
        object.material && object.material.dispose();
        this.scene.remove(object);
    };

    Viewer.prototype.addObject = function(object) {
        this.scene.add(object);
        return object;
    };

    Viewer.prototype.replaceObject = function(oldObject, newObject) {
        oldObject && this.removeObject(oldObject);
        this.addObject(newObject);
        return newObject;
    };

    // -------------------------------------------------------------------------

    Viewer.prototype.render = function() {
        this.renderer.render(this.scene, this.camera);
    };

    Viewer.prototype.screenshot = function(callback) {
        this.render();
        callback(this.canvas.toDataURL());
    };

    // -------------------------------------------------------------------------

    // global settings
    Viewer.globalSettings = globalSettings;

    // export module
    SLAcer.Viewer = Viewer;

})();
