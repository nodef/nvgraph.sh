const cp = require('child_process');

const URL = 'https://github.com/cppf/nvgraph.sh/releases/download/0.2.0/nvgraph';




function fetchSuper(url) {
  cp.execSync(`wget -nv ${url}`);
}

function makeExec() {
  cp.execSync(`chmod +x nvgraph`);
  cp.execSync(`chmod +x index.sh`);
}

function main() {
  fetchSuper(URL);
  makeExec();
}
main();
