import * as THREE from 'three';
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import GUI from 'three/addons/libs/lil-gui.module.min.js'; // si tu veux lil-gui depuis CDN
import { FontLoader } from 'three/addons/loaders/FontLoader.js';
import { TextGeometry } from 'three/addons/geometries/TextGeometry.js';

const codeString = "404";

const holoVertex = `
    uniform float uTime;

    varying vec3 vPosition;
    varying vec3 vNormal;

    float random2D(vec2 value)
    {
        return fract(sin(dot(value.xy, vec2(12.9898,78.233))) * 43758.5453123);
    }

    void main()
    {
        // Position
        vec4 modelPosition = modelMatrix * vec4(position, 1.0);

        // Glitch
        float glitchTime = uTime - modelPosition.y;
        float glitchStrength = sin(glitchTime) + sin(glitchTime * 3.45) +  sin(glitchTime * 8.76);
        glitchStrength /= 3.0;
        glitchStrength = smoothstep(0.3, 1.0, glitchStrength);
        glitchStrength *= 0.25;
        modelPosition.x += (random2D(modelPosition.xz + uTime) - 0.5) * glitchStrength;
        modelPosition.z += (random2D(modelPosition.zx + uTime) - 0.5) * glitchStrength;

        // Final position
        gl_Position = projectionMatrix * viewMatrix * modelPosition;

        // Model normal
        vec4 modelNormal = modelMatrix * vec4(normal, 0.0);

        // Varyings
        vPosition = modelPosition.xyz;
        vNormal = modelNormal.xyz;
    }

`

const holoFragment = `
    uniform vec3 uColor;
    uniform float uTime;

    varying vec3 vPosition;
    varying vec3 vNormal;

    void main()
    {
        // Normal
        vec3 normal = normalize(vNormal);
        if(!gl_FrontFacing)
            normal *= - 1.0;

        // Stripes
        float stripes = mod((vPosition.y - uTime * 0.02) * 20.0, 1.0);
        stripes = pow(stripes, 3.0);


        // Fresnel
        vec3 viewDirection = normalize(vPosition - cameraPosition);
        float fresnel = dot(viewDirection, normal) + 1.0;
        fresnel = pow(fresnel, 2.0);

        // Falloff
        float falloff = smoothstep(0.8, 0.2, fresnel);

        // Holographic
        float holographic = stripes * fresnel;
        holographic += fresnel * 1.25;
        holographic *= falloff;

        // Final color
        gl_FragColor = vec4(uColor, holographic);
        #include <tonemapping_fragment>
        #include <colorspace_fragment>
    }

`


/**
 * Base
 */
// Debug
const gui = new GUI()

// Canvas
const canvas = document.querySelector('canvas.webgl')

// Scene
const scene = new THREE.Scene()

// Loaders
const gltfLoader = new GLTFLoader()
const fontLoader = new FontLoader()

/**
 * Sizes
 */
const sizes = {
    width: window.innerWidth,
    height: window.innerHeight
}

window.addEventListener('resize', () =>
{
    // Update sizes
    sizes.width = window.innerWidth
    sizes.height = window.innerHeight

    // Update camera
    camera.aspect = sizes.width / sizes.height
    camera.updateProjectionMatrix()

    // Update renderer
    renderer.setSize(sizes.width, sizes.height)
    renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2))
})

/**
 * Camera
 */
// Base camera
const camera = new THREE.PerspectiveCamera(25, sizes.width / sizes.height, 0.1, 100)
camera.position.set(0, 0, 50)
scene.add(camera)

// Controls
const controls = new OrbitControls(camera, canvas)
controls.enableDamping = true

/**
 * Renderer
 */
const rendererParameters = {}
rendererParameters.clearColor = '#1d1f2a'

const renderer = new THREE.WebGLRenderer({
    canvas: canvas,
    antialias: true
})
renderer.setClearColor(rendererParameters.clearColor)
renderer.setSize(sizes.width, sizes.height)
renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2))

gui
    .addColor(rendererParameters, 'clearColor')
    .onChange(() =>
    {
        renderer.setClearColor(rendererParameters.clearColor)
    })

/**
 * Material
 */
const materialParameters = {}
materialParameters.color = '#70c1ff'

gui
    .addColor(materialParameters, 'color')
    .onChange(() =>
    {
        codeMaterial.uniforms.uColor.value.set(materialParameters.color)
    })

const codeMaterial = new THREE.ShaderMaterial({
    vertexShader: holoVertex,
    fragmentShader: holoFragment,
    uniforms:
    {
        uTime: new THREE.Uniform(0),
        uColor: new THREE.Uniform(new THREE.Color(materialParameters.color)),
    },
    transparent: true,
    side: THREE.DoubleSide,
    depthWrite: false,
})

/**
 * Objects
 */
let code1 = null;
let code2 = null;
let code3 = null;

// Code
fontLoader.load(
    '/default/static/fonts/helvetiker_regular.typeface.json',
    (font) =>
    {
        const textGeometry1 = new TextGeometry(
            Array.from(codeString)[0],
            {
                font: font,
                size: 2,
                depth: 0.4,
                curveSegments: 20,
                bevelEnabled: true,
                bevelThickness: 0.03,
                bevelSize: 0.02,
                bevelOffset: 0,
                bevelSegments: 5
            }
        )
        const textGeometry2 = new TextGeometry(
            Array.from(codeString)[1],
            {
                font: font,
                size: 2,
                depth: 0.4,
                curveSegments: 20,
                bevelEnabled: true,
                bevelThickness: 0.03,
                bevelSize: 0.02,
                bevelOffset: 0,
                bevelSegments: 5
            }
                )
        const textGeometry3 = new TextGeometry(
            Array.from(codeString)[2],
            {
                font: font,
                size: 2,
                depth: 0.4,
                curveSegments: 20,
                bevelEnabled: true,
                bevelThickness: 0.03,
                bevelSize: 0.02,
                bevelOffset: 0,
                bevelSegments: 5
            }
        )
        textGeometry3.computeBoundingBox();
        textGeometry3.computeVertexNormals();
        textGeometry1.center()

        textGeometry2.center()

        textGeometry3.center()

        code1 = new THREE.Mesh(textGeometry1, codeMaterial)
        code2 = new THREE.Mesh(textGeometry2, codeMaterial)
        code3 = new THREE.Mesh(textGeometry3, codeMaterial)
        code1.position.x = -2
        code3.position.x = 2
        scene.add(code1, code2, code3)
    }
)


/**
 * Animate
 */
const clock = new THREE.Clock()

const tick = () =>
{
    const elapsedTime = clock.getElapsedTime()

    codeMaterial.uniforms.uTime.value = elapsedTime
    // Rotate objects
    if(code1 && code2 && code3)
    {
        code1.rotation.x = - elapsedTime * (0.1 * 4.0)
        code1.rotation.y = elapsedTime * (0.2 * 4.0)
        code2.rotation.x = - elapsedTime * (0.1 * 4.0)
        code2.rotation.y = elapsedTime * (0.2 * 4.0)
        code3.rotation.x = - elapsedTime * (0.1 * 4.0)
        code3.rotation.y = elapsedTime * (0.2 * 4.0)
    }

    // Update controls
    controls.update()

    // Render
    renderer.render(scene, camera)

    // Call tick again on the next frame
    window.requestAnimationFrame(tick)
}

tick()
