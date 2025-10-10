// MOUSE TRACKER FOR DTEKV EMULATOR
// Tracks mouse movement over the VGA area and sends "x,y\n" to UART input
// Will be added to the browser console after the main.bin is running

(function () {
const VGA_WIDTH = 320;
const VGA_HEIGHT = 240;
const MOUSE_BUF_ADDR = 0x00010000; // memory-mapped address for mouse_buf

function connectMouseBuffer() {
  if (!window.dtekv || !window.dtekv.memory) {
    console.log(" Waiting for emulator memory...");
    setTimeout(connectMouseBuffer, 500);
    return;
}


const mem = window.dtekv.memory;
const vgaCanvas = document.querySelector("canvas");

if (!vgaCanvas) {
  console.error(" VGA canvas not found");
  return;
}

vgaCanvas.addEventListener("mousemove", (e) => {
  const rect = vgaCanvas.getBoundingClientRect();

  // Scale mouse coordinates to 320x240
  let x = Math.floor(((e.clientX - rect.left) / rect.width) * VGA_WIDTH);
  let y = Math.floor(((e.clientY - rect.top) / rect.height) * VGA_HEIGHT);

  // Clamp values
  x = Math.max(0, Math.min(VGA_WIDTH - 1, x));
  y = Math.max(0, Math.min(VGA_HEIGHT - 1, y));

  // Write directly to the shared memory (like mouse_buf)
  const view = new DataView(mem.buffer);
  view.setInt32(MOUSE_BUF_ADDR, x, true);
  view.setInt32(MOUSE_BUF_ADDR + 4, y, true);
});

console.log(" Mouse tracking active — writing to emulator memory (mouse_buf)!");

}

connectMouseBuffer();
})();

