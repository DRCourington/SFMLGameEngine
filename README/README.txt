Tile Entity Specification:
Tile N GX GY
  Animation Name   N        std::string (Animation asset name for this tile)
  GX Grid X Pos    GX       float
  GY Grid Y Pos    GY       float

Decoration Entity Specification:
Dec N X Y
  Animation Name   N        std::string (Animation asset name for this tile)
  X Position       X        float
  Y Position       Y        float

Player Specification
Player GX GY CW CJ SX SU SM GY B
  GX, GY Grid Pos  X, Y     float, float (starting pos of player)
  BoundingBox W/H  CW, CH   float, float
  Left/Right Speed SX       float
  Jump Speed       SY       float
  Max Speed        SM       float
  Gravity          GY       float
  Bullet Animation B        std::string (Anim asset to use for bullets)

Assignment Hints

I recommend approaching this assignment in the following order, which wil lhelp you debug your program along the way with minimal errors. Remember to do one step at a time and test whether what you have just implemented is working properly before moving on to any additional steps.
- Renedering system has already been set up for you, to help you debug:
  You can press the T key to toggle drawing Textures
  You can press the C key to toggle drawing bounding boxes
  You can press the G key to toggle drawing the grid
- You can implement Animation::update() and Animation::hasEnded() at any time, it will not affet teh gameplay mechanics whatsoever, just animation
- Implement Scene_play::loadLevel()
Since rendering is already completed once you correctly read in the different types of entites, add them to the EntityManger and they should automatically be drawn to the screen. Add the correct bounding boxes to Tile entities, and no bounding boxes to the Dec Entities. Remember you can toggle debug viewing of bounding boxed with the T and C keys

As part of this step, implement the Scene_Play::gridToMidPixel() function, which takes in as parameters a grid x,y position and an Entity, and returns the Vec2 position of teh CENTER of that Entity. You must use the Animation size of teh Entity to determine where its center point should be. Keep in mind that this means your Entity MUST have its CAnimation component added first, so that it can be used to calculate the midpoint in this function.

- Implement Scene_Play::spawnPlayer()
Read the player configuration from the level file and spawn the player This is where the player should restart when they die.

- Implement some basic WASD u/l/d/r movement for the player entity so that you can use this to help test collisions in the future. Remember that you must use registerAction to register a new action for the scene. See the actions already registerd for you and the sDoAction() fucntion for syntax on how to perform actions.

- Implement Scene_Play::spawnBullet()
Bullet should shoot when Space is pressed in same direction player facing
Holding down the space button should NOT continuously fire bullets. A new bullet can only be fired after the space key has been released Use the entity's CInput.canShoot variable to implement this

- Implement Physics::GetOverlap()
This function should return the overlaop dimenstions between the bounding voxes of two entities. This is the same as the purple rectangle in notes. GetPreviousOverlap should be a copy/paste of this solution except using the previous positions instead of th ecurren tpositions of the entity. If either input entity has no bounding box, then return Vec2(0,0)

- Implement collision checking with bullets / brick tiles such that the brick is destroyed when a bullet colides with it. Remember, a collision occurs when the overlap is non-zero in both the X and Y component. Bullets should always be destroyed when they collide with any non-decorative tile

- Implement collision resolution such that when the player collides with a non-decorative tile, the player cannot enter it or overlap it. When the player collides with a tile from below, its y-velocity should be set to zero so that it fall back downward and doesn't 'hover' below the tile

- Implement a way of detecting which side the layer collided with the tile

- Change the controls such that they are the proper left/right/jump style
NOTE: All movement logic should be in the movement system. The sDoAction system is ONLY used to set the proper CInput Variables. If you modify the players speed or posisition anywhere inside the sDoAction system, you will lose marks as this is potentially unsafe/ produces unwanted effects

- Implement gravity such that the player falls toward the bottom of the screen and land on tiles when it collides with a tile from above. Note that when the player lands on a tile from above, you should set its vertical speed to zero so that gravity does not continue to accelerate the player downward
 
