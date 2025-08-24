#include "../include/Scene_Play.h"
#include "../include/Physics.h"
#include "../include/Assets.h"
#include "../include/GameEngine.h"
#include "../include/Components.h"
#include "../include/Action.h"

#include <iostream>
#include <fstream>

Scene_Play::Scene_Play(GameEngine * gameEngine, const std::string & levelPath)
    : Scene(gameEngine)
    , m_levelPath(levelPath)
    , m_gridText(gameEngine->assets().getFont("Tech"),"", 12)
{
}

void Scene_Play::init()
{

    registerAction(static_cast<int>(sf::Keyboard::Scancode::W),      "UP");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Up),     "UP");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::S),      "DOWN");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Down),   "DOWN");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::A),      "LEFT");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Left),   "LEFT");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::D),      "RIGHT");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Right),  "RIGHT");

    registerAction(static_cast<int>(sf::Keyboard::Scancode::P),      "PAUSE");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Escape), "QUIT");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::T),      "TOGGLE_TEXTURE");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::C),      "TOGGLE_COLLISION");
    registerAction(static_cast<int>(sf::Keyboard::Scancode::G),      "TOGGLE_GRID");


    std::ifstream fin(m_levelPath);
    if (!fin)
    {
        std::cerr << "[Level] Missing '" << m_levelPath << "', using fallback\n";

        // Minimal test entity so you see something
        auto e = m_entityManager.addEntity("debug");
        e->addComponent<CTransform>(Vec2(200.f, 200.f));
        e->addComponent<CBoundingBox>(Vec2(64.f, 64.f));
        e->addComponent<CShape>(Vec2{64.f, 64.f}, sf::Color::Green, sf::Color::Black, 2.f);
        m_player = e;
        m_player->addComponent<CInput>();
        m_player->addComponent<CGravity>(Vec2{0.f, 0.2f});

        // add the animation
        const Animation& idle = m_game->assets().getAnimation("Idle");
        m_player->addComponent<CAnimation>(idle, /*repeat=*/false);
        auto& tf = m_player->getComponent<CTransform>();
        tf.scale = {4.f, 4.f};

        // select the idle sprite on the sheet
        // adjust these to your sheet’s grid:
        const int frameW  = 32;   // try 32; if wrong, use 24 or 16
        const int frameH  = 32;
        const int idleCol = 3;    // column index of the idle frame
        const int idleRow = 0;    // row index of the idle frame

        sf::IntRect rect(sf::Vector2i{idleCol * frameW, idleRow * frameH},
                        sf::Vector2i{frameW, frameH});

        auto& anim = m_player->getComponent<CAnimation>().animation;
        auto& spr  = anim.getSprite();
        spr.setTextureRect(rect);
        spr.setOrigin(sf::Vector2f{frameW * 0.5f, frameH * 0.5f});

        spawnBlock(120.f, 360.f, 0, 0, 3.f);   // (px,py, col,row, scale)
        spawnBlock(120.f+48.f, 360.f, 0, 0, 3.f);
        spawnBlock(120.f+96.f, 360.f, 0, 0, 3.f);

        return;
    }

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Tech"));
}

Vec2 Scene_Play::gridToMidPixel(float gx, float gy, std::shared_ptr<Entity>) {
    return { gx * m_gridSize.x + m_gridSize.x * 0.5f,
             gy * m_gridSize.y + m_gridSize.y * 0.5f };
}

void Scene_Play::loadLevel(const std::string & filename)
{
    // reset the entity manager every time we load a level
    m_entityManager = EntityManager();

    // TODO: read in the level file and add the appropiate entites
    //       use the PlayerConfig struct m_playerConfig to store player properties
    //       this struct is defined at the top of Scene_Play.h

    // NOTE: all of the code below is sample code which shows you how to
    //       set up and use entities with the new syntax, it should be removed

    spawnPlayer();

    // some sample entities
    auto brick = m_entityManager.addEntity("tile");
    // IMPORTANT: always add CAnimation component first so that gridToMidPixel can compute
    brick->addComponent<CAnimation>(m_game->assets().getAnimation("Brick"), true);
    brick->addComponent<CTransform>(Vec2(96, 480));
    // NOTE: Your final code should position the entity with the grid x,y position read from
    // brick->addComponent<CTransform>(gridToMidPixel(gridX, gridY, brick);

    if (brick->getComponent<CAnimation>().animation.getName() == "Brick")
    {
        std::cout << "This could be a good way of identifying if a tile is a brick!\n";
    }

    auto block = m_entityManager.addEntity("tile");
    block->addComponent<CAnimation>(m_game->assets().getAnimation("Block"), true);
    block->addComponent<CTransform>(Vec2(224, 480));
    // add a bounding box, this will now show up if we press the 'C' key
    block->addComponent<CBoundingBox>(m_game->assets().getAnimation("Block").getSize());

    auto question = m_entityManager.addEntity("tile");
    question->addComponent<CAnimation>(m_game->assets().getAnimation("Question"), true);
    question->addComponent<CTransform>(Vec2(352, 480));

    // NOTE: THIS IS INCREDIBLY IMPORTANT PLESE READ THIS EXAMPLE
    //       Components are now returned as reference rather than pointers
    //       if you do not specify  a reference variable type, it will COPY the component
    //       here is an example:
    //
    //       This will COPY the transform into the variable 'transform1' - it is INCORRECT
    //       Any changes you make to transform1 will not be changed inside the entity
    //       auto transform1 = entity->get<CTransform>()
    //
    //       This will REFERENCE the transform  with the variable 'transform2' - it is CORRECT
    //       Now any changes you  make to transform2 will be changed inside the entity
    //       auto& transform2 = entity->get<CTransform>()
}

void Scene_Play::spawnPlayer()
{
    // here is a sample player entity which you can use to construct other entites
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
    m_player->addComponent<CTransform>(Vec2(224, 352));
    m_player->addComponent<CBoundingBox>(Vec2(48, 48));

    // TODO: be sure to add the remaining components to the player
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
    // TODO: this should spawn a bullet at the given entity, going in the direction the entity is facing
}

namespace {
constexpr int TILE_W = 16;      // adjust if your sheet uses 32
constexpr int TILE_H = 16;
}

void Scene_Play::spawnBlock(float px, float py, int col, int row, float scale)
{
    auto e = m_entityManager.addEntity("tile");
    e->addComponent<CTransform>(Vec2(px, py));
    e->getComponent<CTransform>().scale = {2.f, 2.f};

    // collision box matches visual size
    e->addComponent<CBoundingBox>(Vec2{TILE_W * scale, TILE_H * scale});

    // 1-frame anim using the sheet, then crop to one tile
    const Animation& sheet = m_game->assets().getAnimation("BlocksSheet");
    e->addComponent<CAnimation>(sheet, false);

    sf::IntRect rect(sf::Vector2i{col * TILE_W, row * TILE_H},
                    sf::Vector2i{TILE_W,       TILE_H});
    auto& spr = e->getComponent<CAnimation>().animation.getSprite();
    spr.setTextureRect(rect);
    spr.setOrigin(sf::Vector2f{TILE_W * 0.5f, TILE_H * 0.5f});
}



void Scene_Play::update()
{
    m_entityManager.update();

    // TODO: implement pause functionality

    sMovement();
    sLifespan();
    sCollision();
    //sAnimation();
    sRender();

    if (m_player)
    {
        auto& tf = m_player->getComponent<CTransform>();
        auto& in = m_player->getComponent<CInput>();

        const float accel     = 0.8f;
        const float maxSpeedX = 6.0f;
        const float maxSpeedY = 12.0f;
        const float damping   = 0.88f;

        Vec2 a{0.f, 0.f};
        if (in.left)  a.x -= accel;
        if (in.right) a.x += accel;
        if (in.up)    a.y -= accel;
        if (in.down)  a.y += accel;

        if (m_player->hasComponent<CGravity>())
        {
            a.y += m_player->getComponent<CGravity>().gravity.y;
        }

        tf.velocity.x += a.x;
        tf.velocity.y += a.y;

        if (tf.velocity.x >  maxSpeedX) tf.velocity.x =  maxSpeedX;
        if (tf.velocity.x < -maxSpeedX) tf.velocity.x = -maxSpeedX;
        if (tf.velocity.y >  maxSpeedY) tf.velocity.y =  maxSpeedY;
        if (tf.velocity.y < -maxSpeedY) tf.velocity.y = -maxSpeedY;

        // simple, unnormalized movement
        tf.pos.x += tf.velocity.x;
        tf.pos.y += tf.velocity.y;

        tf.velocity.x *= damping;
        tf.velocity.y *= damping;
    }


}

void Scene_Play::sMovement()
{
    Vec2 playerVelocity(0, m_player->getComponent<CTransform>().velocity.y);

    if (m_player->getComponent<CInput>().up)
    {
        m_player->getComponent<CState>().state = "run";
        playerVelocity.y = -3;
    }

    m_player->getComponent<CTransform>().velocity = playerVelocity;

    for (auto e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CGravity>())
        {
            // if the playeris moving faster than max speed in any direction
            // set its speed in that direction to the max speed
        }

        e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;
    }

    // TODO: Implement player movement / jumping based on its CInput component
    // TODO: Implement gravity's effect on the player
    // TODO: Implement the maximum player speed in both X and Y directions
    // Note: Setting an entity's scale.x to -1/1 will make it face to the left/right
}

void Scene_Play::sLifespan()
{
    //TODO: Check lifespan of entities that have them, and destroy them if they go over
}

void Scene_Play::sCollision()
{
    if (!m_player || !m_player->hasComponent<CBoundingBox>()) return;

    auto& ptf = m_player->getComponent<CTransform>();
    auto& pbb = m_player->getComponent<CBoundingBox>();

    bool onGround = false;

    // player vs tiles
    for (auto& t : m_entityManager.getEntities("tile")) {
        if (!t->hasComponent<CBoundingBox>()) continue;

        Vec2 ov  = Physics::GetOverlap(m_player, t);
        if (ov.x <= 0.f || ov.y <= 0.f) continue;

        const auto& ttf = t->getComponent<CTransform>();
        const auto& tbb = t->getComponent<CBoundingBox>();

        // center deltas (account for offsets)
        const Vec2 pc{ ptf.pos.x + pbb.offset.x, ptf.pos.y + pbb.offset.y };
        const Vec2 tc{ ttf.pos.x + tbb.offset.x, ttf.pos.y + tbb.offset.y };
        const float dx = tc.x - pc.x;
        const float dy = tc.y - pc.y;

        // choose axis with smaller penetration
        if (ov.y < ov.x) {
            if (dy > 0.f) {           // tile is below -> land on top
                ptf.pos.y -= ov.y;
                onGround = true;
            } else {                   // hit head on tile above
                ptf.pos.y += ov.y;
            }
            ptf.velocity.y = 0.f;
        } else {
            if (dx > 0.f) ptf.pos.x -= ov.x;   // tile is right
            else          ptf.pos.x += ov.x;   // tile is left
            ptf.velocity.x = 0.f;
        }
    }

    // update grounded/air state
    if (m_player->hasComponent<CState>()) {
        auto& st = m_player->getComponent<CState>();
        st.prev  = st.state;
        st.state = onGround ? "ground" : "air";
    }

    // keep player within left boundary
    if (ptf.pos.x - pbb.halfSize.x < 0.f) {
        ptf.pos.x = pbb.halfSize.x;
        ptf.velocity.x = 0.f;
    }

    // fell in a hole (below screen)
    if (ptf.pos.y - pbb.halfSize.y > static_cast<float>(height())) {
        // simple respawn; adjust to your spawn system
        ptf.pos = Vec2{200.f, 200.f};
        ptf.velocity = Vec2{0.f, 0.f};
    }

    // bullets vs tiles (optional; requires entities tagged "bullet")
    for (auto& b : m_entityManager.getEntities("bullet")) {
        if (!b->hasComponent<CBoundingBox>()) continue;

        for (auto& t : m_entityManager.getEntities("tile")) {
            if (!t->hasComponent<CBoundingBox>()) continue;

            Vec2 ov = Physics::GetOverlap(b, t);
            if (ov.x <= 0.f || ov.y <= 0.f) continue;

            b->destroy();
            if (t->hasComponent<CAnimation>()) {
                auto& ca = t->getComponent<CAnimation>();
                const std::string n = !ca.name.empty() ? ca.name : ca.animation.getName();
                if (n == "Brick") t->destroy();
            }
            break;
        }
    }
}

void Scene_Play::sDoAction(const Action& action)
{
    if (!m_player) return;

    auto& in = m_player->getComponent<CInput>();

    if (action.isStart()) {
        if (action.name() == "UP")    in.up    = true;
        if (action.name() == "DOWN")  in.down  = true;
        if (action.name() == "LEFT")  in.left  = true;
        if (action.name() == "RIGHT") in.right = true;

        if (action.name() == "TOGGLE_TEXTURE")   m_drawTextures  = !m_drawTextures;
        if (action.name() == "TOGGLE_COLLISION") m_drawCollision = !m_drawCollision;
        if (action.name() == "TOGGLE_GRID")      m_drawGrid      = !m_drawGrid;
    } else {
        if (action.name() == "UP")    in.up    = false;
        if (action.name() == "DOWN")  in.down  = false;
        if (action.name() == "LEFT")  in.left  = false;
        if (action.name() == "RIGHT") in.right = false;
    }
}

void Scene_Play::sAnimation()
{
    // TODO: Complete the Animation class code first

    if (m_player->getComponent<CState>().state == "air")
    {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Air"));
    }

    if (m_player->getComponent<CState>().state == "run")
    {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"));
    }
    // TODO: set the animation of the player based on its CState component
    // TODO: fot each entity with an animation, call entity->getComponent<CAnimation>().animation.update()
    //       if the animation is not repeated, and it has ended, destroy the entity
}

void Scene_Play::onEnd()
{
    // TODO: When the scene ends, change back to the MENU scene
    //       use m_game->changeScene(correct params);
}
void Scene_Play::sRender() {
    auto& win = m_game->window();

    // set camera
    {
        auto size = win.getSize();
        sf::View view = win.getView();
        if (m_player && m_player->hasComponent<CTransform>()) {
            const auto& p = m_player->getComponent<CTransform>().pos;
            float cx = std::max(size.x * 0.5f, p.x);
            view.setCenter(sf::Vector2f{cx, size.y * 0.5f});
        } else {
            view.setCenter(sf::Vector2f{size.x * 0.5f, size.y * 0.5f});
        }
        win.setView(view);
    }

    // draw entities once, honoring toggles
    for (auto& e : m_entityManager.getEntities()) {
        auto& tf = e->getComponent<CTransform>();
        auto& ca = e->getComponent<CAnimation>();
        auto& sh = e->getComponent<CShape>();

        if (m_drawTextures && ca.has) {
            auto& spr = ca.animation.getSprite();
            spr.setPosition(sf::Vector2f{tf.pos.x, tf.pos.y});
            spr.setScale   (sf::Vector2f{tf.scale.x, tf.scale.y});
            spr.setRotation(sf::degrees(tf.angle));
            win.draw(spr);
        } else if (sh.has && sh.shape) {
            sh.shape->setPosition(sf::Vector2f{tf.pos.x, tf.pos.y});
            sh.shape->setScale   (sf::Vector2f{tf.scale.x, tf.scale.y});
            sh.shape->setRotation(sf::degrees(tf.angle));
            win.draw(*sh.shape);
        }
    }

    // collision boxes
    if (m_drawCollision) {
        for (auto& e : m_entityManager.getEntities()) {
            if (!e->hasComponent<CBoundingBox>()) continue;
            const auto& box = e->getComponent<CBoundingBox>();
            const auto& tr  = e->getComponent<CTransform>();
            sf::RectangleShape r;
            r.setSize(sf::Vector2f{box.size.x - 1.f, box.size.y - 1.f});
            r.setOrigin(sf::Vector2f{box.halfSize.x, box.halfSize.y});
            r.setPosition(sf::Vector2f{tr.pos.x, tr.pos.y});
            r.setFillColor(sf::Color(0,0,0,0));
            r.setOutlineColor(sf::Color(255,255,255,255));
            r.setOutlineThickness(1.f);
            win.draw(r);
        }
    }

    // grid
    if (m_drawGrid) {
        float leftX   = 0.f;
        float rightX  = static_cast<float>(width());
        float nextGX  = leftX - std::fmod(leftX, m_gridSize.x);

        for (float x = nextGX; x < rightX; x += m_gridSize.x)
            drawLine(Vec2{x, 0.f}, Vec2{x, static_cast<float>(height())});

        for (float y = 0.f; y < height(); y += m_gridSize.y) {
            drawLine(Vec2{leftX, static_cast<float>(height()) - y},
                     Vec2{rightX, static_cast<float>(height()) - y});
            for (float x = nextGX; x < rightX; x += m_gridSize.x) {
                std::string xCell = std::to_string(static_cast<int>(x / m_gridSize.x));
                std::string yCell = std::to_string(static_cast<int>(y / m_gridSize.y));
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(sf::Vector2f{x + 3.f, static_cast<float>(height()) - y - m_gridSize.y + 2.f});
                win.draw(m_gridText);
            }
        }
    }
}
