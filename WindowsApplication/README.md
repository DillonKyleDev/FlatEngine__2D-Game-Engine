## FlatEngine - 2D Game Engine

## Description:

This is a 2D game engine made using SDL_2 and ImGui.

## Purpose

This project is both a passion project and a way to improve my understanding of game engines and enhance my overall development toolbelt.  I will use this game engine to replace Unity in all my future 2D game development projects.  I will also be using the skills I learn in the development of this game engine to help me eventually develop a full 3D game engine in the future.

## Design Inspiration

I've spent a lot of time with the Unity game engine and so have found comfort in its design philosophy.  I will be adopting many of Unitys methods for handling various things within my engine.


## Moving Parts

## SDL_2

The engine is built using SDL_2 as a base, along with several other SDL libraries including SDL_ttf for fonts, SDL_Image, and SDL_Mixer for audio.

## ImGui

The entire user interface, including all of the interactions within the scene view, are handled using ImGui_Docking.

## Logging

For logging, I created my own library of functions that interface with ImGui.

## Scenes

Scenes are the container for all GameObjects and are saved in JSON format using a JSON Formatting library for simple parsing.

## Components

Mirroring Unitys design, FlatEngines GameObjects require components be attached for their functionality.  The current list of components included in FlatEngine are:

1. Transform
2. Sprite
3. Camera
4. Button
5. Text
6. Canvas
7. Audio
8. Script

These function as you might expect they would.

These components are all that are needed if you aren't using any kind of physics in your 2D game, however, that is something I would like to add in the future.  Some components I would like to add in the future include:

1. Box Collider
2. Circle Collider
3. RigidBody