#pragma once

// How to handle scenes?
//
// Because of hot reloading we cannot use any function pointers.
// That means, no interface classes or class based polymorphism.
//
// A Scene is state + initialize() + update() + render()
//
// e.g. struct MainMenuScene, MainMenuScene::initialize, MainMenuScene::update, MainMenuScene::render
//
// Store as list of disjunctions? [Scene] where Scene = MainMenuScene | GameplayScene | etc.

class SceneManager {
	//
};
