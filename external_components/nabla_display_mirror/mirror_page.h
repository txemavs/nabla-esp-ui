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
let token='',busy=false,allowInput=false;
fetch("/mirror/capabilities").then(r=>r.json()).then(c=>{allowInput=c.input;document.querySelector(".controls").hidden=!allowInput;document.querySelector("main p").textContent=allowInput?"La ruleta y estos controles manejan la misma pantalla.":"Solo lectura: controla la pantalla desde el dispositivo."});
async function action(value){if(!allowInput||busy||!token)return;busy=true;try{
const r=await fetch('/mirror/action',{method:'POST',headers:{'X-Nabla-Token':token},body:new URLSearchParams({action:value})});
if(!r.ok)throw Error();}catch{status.textContent='No se pudo enviar el control'}finally{busy=false}}
document.querySelectorAll('[data-action]').forEach(b=>b.onclick=()=>action(b.dataset.action));
document.addEventListener('keydown',e=>{const a={ArrowUp:'up',ArrowDown:'down',Enter:'enter',Escape:'back'}[e.key];if(a){e.preventDefault();action(a)}});
async function tick(){
if(!document.hidden)try{
if(!token){const r=await fetch('/mirror/token',{cache:'no-store'});if(!r.ok)throw Error();token=await r.text()}
const r=await fetch('/mirror/frame',{cache:'no-store'});if(!r.ok)throw Error();
const bytes=new Uint8Array(await r.arrayBuffer());if(bytes.length!==1024)throw Error();
const image=ctx.createImageData(128,64);for(let i=0;i<8192;i++){const c=bytes[i>>3]&(128>>(i&7))?255:0;image.data.set([c,c,c,255],i*4)}ctx.putImageData(image,0,0);
status.textContent='En directo · 128 × 64';}catch{token='';status.textContent='Sin conexión · reintentando…'}
setTimeout(tick,100)}
tick();
</script></html>)MIRROR";
}
