#!/usr/bin/env node
// TODO.wait typescript .d.ts generation https://github.com/emscripten-core/emscripten/issues/7083

// console.log(process.cwd());
// import { default as oki }  from "../build/Release/okiidoku.js";
globalThis.okiidokuMain = () => {
	// console.log(oki);
	oki.rng.seed(BigInt(Date.now()));
	{
		const grid = new oki.Grid(3);
		oki.generate(grid, oki.rng.getRngSeed());
		// console.log(grid); // node's default console logging doesn't call toString
		console.log(grid.toString());
		grid.delete();
	}
}
const { default: oki } = await import("../build/Release/okiidoku.js");