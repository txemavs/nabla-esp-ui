// Offline 128x64 canvas viewer; frames come from the same draw pass as the OLED.
#pragma once
namespace esphome::nabla_display_mirror {
inline constexpr char MIRROR_PAGE[]=R"MIRROR(<!doctype html>
<html lang="es"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Nabla · Pantalla</title><style>
[hidden]{display:none!important}
body{margin:0;background:#090a0c;color:#eee;font:18px monospace}
header,footer{padding:18px;background:#16191d}main{padding:24px;max-width:820px;margin:auto}
canvas{width:100%;image-rendering:pixelated;display:block;background:black;aspect-ratio:2}
.controls{display:flex;flex-wrap:wrap;gap:12px;justify-content:center;margin:24px 0}
button{background:#16191d;color:inherit;font:inherit;border:1px solid #555;border-radius:12px;padding:16px}
button:focus-visible{outline:2px solid white}p{line-height:1.5;font-size:14px}
</style><header>▽ Pantalla del dispositivo</header><main>
<canvas width="128" height="64" aria-label="Pantalla real del dispositivo"></canvas>
<div class="controls"><button data-action="up">↑</button><button data-action="down">↓</button><button data-action="enter">Aceptar</button><button data-action="back">Volver</button></div>
<p>La ruleta y estos controles manejan la misma pantalla. Teclado: ↑, ↓, Enter y Escape.</p>
</main><footer id="status">Conectando…</footer><script>
const canvas=document.querySelector('canvas'),ctx=canvas.getContext('2d'),status=document.querySelector('#status');
let token='',busy=false,allowInput=false,allowTouch=false,width=128,height=64,format='mono1',capabilitiesReady=false;
async function capabilities(){const r=await fetch("/mirror/capabilities",{cache:"no-store"});if(!r.ok)throw Error();const c=await r.json();allowInput=c.input;allowTouch=c.touch===true;canvas.style.cursor=allowTouch?"pointer":"default";width=c.width||128;height=c.height||64;format=c.format||"mono1";canvas.width=width;canvas.height=height;canvas.style.aspectRatio=width+"/"+height;capabilitiesReady=true;document.querySelector(".controls").hidden=!allowInput;document.querySelector("main p").textContent=allowInput?"La ruleta y estos controles manejan la misma pantalla.":allowTouch?"Pulsa la imagen para tocar la pantalla. Arrastres no disponibles.":"Solo lectura: controla la pantalla desde el dispositivo.";}
async function action(value){if(!allowInput||busy||!token)return;busy=true;try{
const r=await fetch('/mirror/action',{method:'POST',headers:{'X-Nabla-Token':token},body:new URLSearchParams({action:value})});
if(!r.ok)throw Error();}catch{status.textContent='No se pudo enviar el control'}finally{busy=false}}
canvas.addEventListener('click',async e=>{
if(!allowTouch||busy||!token)return;
const box=canvas.getBoundingClientRect(),x=Math.floor((e.clientX-box.left)*width/box.width),y=Math.floor((e.clientY-box.top)*height/box.height);
if(x<0||y<0||x>=width||y>=height)return;
busy=true;try{const r=await fetch('/mirror/touch',{method:'POST',headers:{'X-Nabla-Token':token},body:new URLSearchParams({x,y})});if(!r.ok){if(r.status===401)token='';throw Error();}}catch{status.textContent='No se pudo enviar el toque'}finally{busy=false}
});
document.querySelectorAll('[data-action]').forEach(b=>b.onclick=()=>action(b.dataset.action));
document.addEventListener('keydown',e=>{const a={ArrowUp:'up',ArrowDown:'down',Enter:'enter',Escape:'back'}[e.key];if(a){e.preventDefault();action(a)}});
async function tick(){
if(!document.hidden)try{
if(!capabilitiesReady)await capabilities();
if(!token){const r=await fetch('/mirror/token',{cache:'no-store'});if(!r.ok)throw Error();token=await r.text()}
const r=await fetch('/mirror/frame',{cache:'no-store'});if(!r.ok)throw Error();
const bytes=new Uint8Array(await r.arrayBuffer());if(bytes.length!==(format==="rgb332"?width*height:Math.ceil(width*height/8)))throw Error();
const image=ctx.createImageData(width,height);for(let i=0;i<width*height;i++){const c=bytes[i>>3]&(128>>(i&7))?255:0;const v=bytes[i];image.data.set(format==="rgb332"?[Math.round((v>>5)*255/7),Math.round(((v>>2)&7)*255/7),Math.round((v&3)*255/3),255]:[c,c,c,255],i*4)}ctx.putImageData(image,0,0);
status.textContent='En directo · '+width+' × '+height;}catch{token='';capabilitiesReady=false;status.textContent='Sin conexión · reintentando…'}
setTimeout(tick,format==="rgb332"?350:100)}
tick();
</script></html>)MIRROR";
}
