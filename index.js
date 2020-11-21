const fs = require('fs');
const path = require('path');
const vsctm = require('vscode-textmate');
const oniguruma = require('vscode-oniguruma');
const readline = require("readline");


/**
 * Utility to read a file as a promise
 */
function readFile(path) {
    return new Promise((resolve, reject) => {
        fs.readFile(path, (error, data) => error ? reject(error) : resolve(data));
    })
}

const wasmBin = fs.readFileSync(path.join(__dirname, './node_modules/vscode-oniguruma/release/onig.wasm')).buffer;
const vscodeOnigurumaLib = oniguruma.loadWASM(wasmBin).then(() => {
    return {
        createOnigScanner(patterns) { return new oniguruma.OnigScanner(patterns); },
        createOnigString(s) { return new oniguruma.OnigString(s); }
    };
});

// Create a registry that can create a grammar from a scope name.
const registry = new vsctm.Registry({
    onigLib: vscodeOnigurumaLib
});
console.log("test")
// Load the JavaScript grammar and any other grammars included by it async.
var stdinBuffer = fs.readFileSync(0); // STDIN_FILENO = 0
file = (stdinBuffer.toString());

def="JavaScript.tmLanguage.json"
readFile(def).then(data => registry.addGrammar(vsctm.parseRawGrammar(data, def))).then(grammar => {
    let ruleStack = vsctm.INITIAL;
    const readInterface = readline.createInterface({
        input: fs.createReadStream(file),
        console: false
    });
    readInterface.on('line', function(line) { 
        const lineTokens = grammar.tokenizeLine(line, ruleStack);
        console.log(`\nTokenizing line: ${line}`);
        for (let j = 0; j < lineTokens.tokens.length; j++) {
            const token = lineTokens.tokens[j];
            console.log(` - token from ${token.startIndex} to ${token.endIndex} ` +
              `(${line.substring(token.startIndex, token.endIndex)}) ` +
              `with scopes ${token.scopes.join(', ')}`
            );
        }
        ruleStack = lineTokens.ruleStack;
    })
});
