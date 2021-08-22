#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <memory>
#include "TextureManager.h"

TextureManager g_textureManager;

///< desciption of the player ship.
class object 
{
private:

    sf::Rect<float> m_rect;
    sf::Vector2f m_speed;
    sf::Vector2f m_pos;
    sf::Vector2u m_size;
    sf::Sprite   m_sprit;
    sf::Texture m_texture;
public:
    //object(std::string texture)
    //{
    //    m_texture.loadFromFile(texture);
    //    m_speed.x = 1;
    //    m_speed.y = 1;
    //    m_pos.x = 20;
    //    m_pos.y = 20;
    //    m_sprit.setTexture(m_texture);
    //    m_size = m_texture.getSize();
    //};
    object(sf::Texture& texture, sf::Vector2f pos, sf::Vector2f speed)
    {
        m_pos = pos;
        m_speed = speed;
        m_sprit.setTexture(texture);
        m_size = texture.getSize();
        m_rect.top = m_pos.y;
        m_rect.left = m_pos.x;
        m_rect.width = m_size.x;
        m_rect.height = m_size.y;
    };
    object(std::string texture, sf::Vector2f pos, sf::Vector2f speed)
    {
        m_texture.loadFromFile(texture);
        m_pos = pos;
        m_speed = speed;
        m_sprit.setTexture(m_texture);
        m_size = m_texture.getSize();
        m_rect.top = m_pos.y;
        m_rect.left = m_pos.x;
        m_rect.width = m_size.x;
        m_rect.height = m_size.y;
    };
    ~object(){};
    void draw(sf::RenderWindow &window)
    {
        m_sprit.setPosition(m_pos);
        window.draw(m_sprit);
    };
    sf::Vector2f GetPos(){ return m_pos; };
    sf::Vector2f GetSpeed(){ return m_speed; };
    void SetPos(sf::Vector2f pos){ 
        m_pos = pos; 
        m_rect.left = m_pos.x; 
        m_rect.top = m_pos.y;
        };
    void SetX(float x){ 
        m_pos.x = x; 
        m_rect.left = m_pos.x;
        };
    void SetY(float y){ 
        m_pos.y = y; 
        m_rect.top = m_pos.y;
        };
    sf::Vector2u GetSize(){ return m_size; };
    sf::Rect<float> GetRect() { return m_rect; };
};

const float fps = 60;
const float period = 1000000/fps;
const int width = 800;
const int height = 600;
const float bwidth = 15;
const float bheight = 2;
const float radius = 10;

struct Bullet
{
    sf::Vector2f pos;
    float speed;
};

Bullet createBullet()
{
    Bullet bul;
    sf::Vector2f b(width, rand()% height);
    bul.pos = b;
    bul.speed = rand()% 2;
    bul.speed++;
    return bul;
}

bool isOutX( Bullet b)
{
    if(b.pos.x <= 0)
        return true;
    return false;
}

bool isOutX2( std::shared_ptr<object> b)
{
    if(b->GetPos().x <= -b->GetRect().width){
        return true;
    }
    return false;
}

// CIRCLE/RECTANGLE
bool circleRect(float cx, float cy, float rad, float rx, float ry, float rw, float rh) 
{
  // temporary variables to set edges for testing
  float testX = cx;
  float testY = cy;

  // which edge is closest?
  if (cx < rx)         testX = rx;      // test left edge
  else if (cx > rx+rw) testX = rx+rw;   // right edge
  if (cy < ry)         testY = ry;      // top edge
  else if (cy > ry+rh) testY = ry+rh;   // bottom edge

  // get distance from closest edges
  float distX = cx-testX;
  float distY = cy-testY;
  float distance = sqrt( (distX*distX) + (distY*distY) );

  // if the distance is less than the radius, collision!
  if (distance <= rad) {
    return true;
  }
  return false;
}
// CIRCLE/CIRCLE
bool circleCircle(float c1x, float c1y, float c1r, float c2x, float c2y, float c2r) {

  // get distance between the circle's centers
  // use the Pythagorean Theorem to compute the distance
  float distX = c1x - c2x;
  float distY = c1y - c2y;
  float distance = sqrt( (distX*distX) + (distY*distY) );

  // if the distance is less than the sum of the circle's
  // radii, the circles are touching!
  if (distance <= c1r+c2r) {
    return true;
  }
  return false;
}
int main()
{
    srand(time(NULL));
    //sf::Vector2f pos( 20, 20);  ///< Player position
    //sf::Texture playerTexture;//, shipTexture;
    //playerTexture.loadFromFile("./player.png");
    //shipTexture.loadFromFile("./ship.png");
    //object *player = new object("./player.png", sf::Vector2f(20, 20), sf::Vector2f(0, 0));
    object *player = new object(TextureManager::GetTexture("player.png"), sf::Vector2f(20, 20), sf::Vector2f(0, 0));
    //std::vector<Bullet> bullets;
    std::vector<std::shared_ptr<object>> ship;           
    sf::RenderWindow window(sf::VideoMode( 800, 600), "SFML works!");
    
    //sf::CircleShape shape(radius);
    //shape.setFillColor(sf::Color::Green);
    //shape.setPosition(  _player.GetPos());

    bool gameover = true;
    sf::Text text;
    sf::Font font;
    font.loadFromFile("Pacifico.ttf");
    text.setFont(font);
    text.setCharacterSize(40);
    text.setFillColor(sf::Color::White);

    sf::Clock timer;
    sf::Clock gameTime;

    int score = 0;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (!gameover){     ///< If game is running.
            sf::Time dt = timer.restart();  ///< Frame rate controller.

            ///< Update position of player.
            sf::Vector2f newpos = player->GetPos();
            newpos.y -= (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))      ?2 * 100 * dt.asSeconds():0;
            newpos.y += (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))    ?2 * 100 * dt.asSeconds():0;
            newpos.x -= (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))    ?2 * 100 * dt.asSeconds():0;
            newpos.x += (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))   ?2 * 100 * dt.asSeconds():0;
            if (newpos.x >= 0 && newpos.x <= width-radius * 2)
                player->SetX(newpos.x);
            if (newpos.y > 0 && newpos.y <= height - radius * 2)
                player->SetY(newpos.y);
            //shape.setPosition(  _player.);

            float cx, cy, cr;
            cx = player->GetPos().x+(player->GetSize().x/2);
            cy = player->GetPos().y+(player->GetSize().y/2);
            cr = std::min(player->GetSize().x/2, player->GetSize().y/2);
            ///< Update bullets and check for colitio.
            for (auto& b : ship)
            {
                //b.pos.x-= b.speed * 100 * dt.asSeconds();
                b->SetX(b->GetPos().x - b->GetSpeed().x * 100 * dt.asSeconds());
                float bx, by, br;
                bx = b->GetPos().x+(b->GetSize().x/2);
                by = b->GetPos().y+(b->GetSize().y/2);
                br = std::min(b->GetSize().x/2, b->GetSize().y/2);
                if (circleCircle(cx, cy, cr, bx, by, br))//b->GetRect().intersects((player->GetRect())))//circleRect(newpos.x+radius, newpos.y+radius, radius, b->GetPos().x, b->GetPos().y, bwidth, bheight))//b.pos.x, b.pos.y, bwidth, bheight))
                    gameover = true;
            }
            //std::remove_if(bullets.begin(), bullets.end(), isOutX);
            ship.erase(std::remove_if( std::begin(ship), std::end(ship), isOutX2), ship.end());
            if(rand()% 100 > 98){
                sf::Vector2f pos;
                pos.x = width;
                pos.y = rand()% height;
                sf::Vector2f speed((rand()% 2)+1, 0);
                //ship.push_back( std::shared_ptr<object>(new object("./ship.png", pos, speed)));
                ship.push_back( std::shared_ptr<object>(new object(TextureManager::GetTexture("ship.png"), pos, speed)));
                //bullets.push_back( createBullet());
            }
                

            ///Get time for score.
            sf::Time s = gameTime.getElapsedTime();
            score = s.asMilliseconds()/100;
            text.setString(std::to_string(score));
            text.setPosition(width/2 -50, 5);
        }
        else {  ///< If game over.
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                player->SetPos(sf::Vector2f(20, height / 2));//pos.x = 20;
                //pos.y = height / 2;
                //bullets.clear();
                ship.clear();
                gameover = false;
                gameTime.restart();
                score = 0;
            }
            text.setString("Game Over\r\nYour Score: " + std::to_string(score));
            text.setPosition(width/2 -50, 5);
        }

        ///< Draw Everyting.
        window.clear();
        //for (auto& b : bullets)
        for (auto& b : ship)
        {
            //sf::RectangleShape r;
            //r.setFillColor(sf::Color::Red);
            //r.setPosition(b.pos);
            //r.setSize(sf::Vector2f(bwidth, bheight));
            //window.draw(r);
            b->draw(window);
        }
        player->draw(window);
        //window.draw(shape);
        window.draw(text);
        window.display();
        
    }
    return 0;
}