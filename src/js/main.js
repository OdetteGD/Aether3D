import * as THREE from "three";
import "../css/app.css";
import {getEngineVersion} from "./native-bridge.js";
const canvas=document.getElementById("viewport"),status=document.getElementById("status"),version=document.getElementById("version");
function setStatus(value,ready=false){status.textContent=value;status.classList.toggle("ready",ready);}
async function boot(){
 const renderer=new THREE.WebGLRenderer({canvas,antialias:true,alpha:false,powerPreference:"high-performance",preserveDrawingBuffer:false});
 renderer.setPixelRatio(Math.min(window.devicePixelRatio||1,2));renderer.setSize(window.innerWidth,window.innerHeight,false);renderer.setClearColor(0x080b10,1);
 const scene=new THREE.Scene(),camera=new THREE.PerspectiveCamera(60,window.innerWidth/window.innerHeight,.05,1000);camera.position.set(0,1.5,5);
 scene.add(new THREE.HemisphereLight(0xffffff,0x182033,2));scene.add(new THREE.GridHelper(20,20,0x31405b,0x1b2433));
 const cube=new THREE.Mesh(new THREE.BoxGeometry(1.2,1.2,1.2),new THREE.MeshStandardMaterial({color:0x4f8cff,roughness:.45,metalness:.2}));cube.position.y=.8;scene.add(cube);
 const resize=()=>{camera.aspect=window.innerWidth/window.innerHeight;camera.updateProjectionMatrix();renderer.setSize(window.innerWidth,window.innerHeight,false);};window.addEventListener("resize",resize,{passive:true});
 try{version.textContent="Core: "+await getEngineVersion();setStatus("Native core online",true);}catch(error){version.textContent="Core: Web fallback";setStatus("Web renderer online");}
 const clock=new THREE.Clock();const frame=()=>{const elapsed=clock.getElapsedTime();cube.rotation.x=elapsed*.35;cube.rotation.y=elapsed*.55;renderer.render(scene,camera);requestAnimationFrame(frame);};resize();frame();
}
document.addEventListener("deviceready",boot,{once:true});if(!window.cordova)boot();
