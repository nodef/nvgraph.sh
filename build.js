const path  = require('path');
const fs    = require('fs');
const cp    = require('child_process');
const build = require('extra-build');
const htmlEntities   = require('html-entities');
const markdownToText = require('markdown-to-text').default;

const owner  = 'nodef';
const repo   = build.readMetadata('.').name;
const NVGRAPH_URL = 'https://github.com/nodef/nvgraph.sh/releases/download/1.3.0/nvgraph';
const HELP_URL    = 'https://github.com/nodef/nvgraph.sh/wiki/${name}';
const HELP_NAME_SIZE = 16;
const HELP_DESC_SIZE = 74;




function stringLimit(x, n) {
  return x.length <= n? x : x.substring(0, n-4)+' ...';
}

function stringAlign(x, n, s) {
  var a = '';
  for (var i=0; i<x.length; i+=n) {
    if (a) a += '\n'+' '.repeat(s);
    a += x.substring(i, i+n);
  }
  return a;
}




function fetchRelease(url) {
  cp.execSync(`wget -nv ${url}`);
}


function readDescWiki() {
  var a = new Map();
  for (var f of fs.readdirSync('wiki')) {
    if (path.extname(f) !== '.md') continue;
    if (f==='Home.md') continue;
    var d = build.readFileText(`wiki/${f}`);
    var name = f.replace(/\..*/, '');
    var desc = d.match(/^(.*)$/m)[1];
    a.set(name, desc);
  }
  return a;
}


function readHelp() {
  var a = '', ns = HELP_NAME_SIZE, ds = HELP_DESC_SIZE;
  for (var [name, desc] of readDescWiki())
    a += ` ${name.padEnd(ns)} ${stringAlign(desc, ds, 2+ns)}\n`;
  var d = build.readFileText('man/help.txt');
  return d.replace('${commands}', a);
}


function readIndex(descs) {
  var a = '';
  var names = [...descs.keys()].sort();
  for (var name of names) {
    var desc = descs.get(name);
    a += `| [${name}] | ${stringLimit(desc, HELP_DESC_SIZE)} |\n`;
  }
  a += `\n\n`;
  for (var name of descs.keys())
    a += `[${name}]: ${HELP_URL.replace('${name}', name)}\n`;
  return a;
}


function copyMan(dir) {
  fs.mkdirSync('man', {recursive: true});
  if (!fs.existsSync(dir)) return;
  for (var f of fs.readdirSync(dir)) {
    if (path.extname(f) !== '.md') continue;
    if (f === 'Home.md') continue;
    var g = f.replace(/\.md$/g, '');
    if (fs.existsSync(`man/${g}.txt`)) continue;
    var d = build.readFileText(`${dir}/${f}`);
    d = markdownToText(d);
    d = htmlEntities.decode(d);
    build.writeFileText(`man/${g}.txt`, d);
  }
}


function makeExec() {
  cp.execSync(`chmod +x nvgraph`);
  cp.execSync(`chmod +x index.sh`);
}




function buildPackage(f=true) {
  var descs = readDescWiki();
  if (f) copyMan('wiki');
  if (f) build.writeFileText('man/help.txt', readHelp());
  build.writeFileText('index.log', readIndex(descs));
  var m = build.readMetadata('.');
  m.keywords = [...new Set([...m.keywords, ...descs.keys()])];
  if (f) build.writeMetadata('.', m);
  if (f) fetchRelease(NVGRAPH_URL);
  if (f) makeExec();
}




// Publish a root package to NPM, GitHub.
function publishRootPackage(ds, ver, typ) {
  var _package = build.readDocument('package.json');
  var _readme  = build.readDocument('README.md');
  var m  = build.readMetadata('.');
  m.version  = ver;
  build.writeMetadata('.', m);
  build.publish('.');
  try { build.publishGithub('.', owner); }
  catch {}
  build.writeDocument(_package);
  build.writeDocument(_readme);
}


// Publish root packages to NPM, GitHub.
function publishRootPackages(ds, ver) {
  buildPackage(true);
  publishRootPackage(ds, ver, '');
}


// Pushish root, sub packages to NPM, GitHub.
function publishPackages(ds) {
  var m   = build.readMetadata('.');
  var ver = build.nextUnpublishedVersion(m.name, m.version);
  publishRootPackages(ds, ver);
}


// Finally.
function main(a) {
  if (a[2]==='publish-packages') publishPackages([]);
  else buildPackage(false);
}
main(process.argv);
