const root = `${import.meta.dir}/../www`;
const port = Number(Bun.env.PORT ?? 5173);

const mime = {
  ".css": "text/css; charset=utf-8",
  ".html": "text/html; charset=utf-8",
  ".js": "text/javascript; charset=utf-8",
  ".json": "application/json; charset=utf-8",
  ".png": "image/png",
  ".svg": "image/svg+xml",
};

function contentType(path) {
  const ext = path.slice(path.lastIndexOf("."));
  return mime[ext] ?? "application/octet-stream";
}

Bun.serve({
  hostname: "0.0.0.0",
  port,
  async fetch(request) {
    const url = new URL(request.url);
    const requestedPath = decodeURIComponent(url.pathname === "/" ? "/index.html" : url.pathname);
    const filePath = `${root}${requestedPath}`;
    const file = Bun.file(filePath);

    if (await file.exists()) {
      return new Response(file, { headers: { "content-type": contentType(filePath) } });
    }

    return new Response(Bun.file(`${root}/index.html`), { headers: { "content-type": mime[".html"] } });
  },
});

console.log(`GreenHouse dev server: http://127.0.0.1:${port}`);
