#include "../include/game.hpp"
#include "../include/ImageFileReader.hpp"

//#include "../include/DebugState.hpp"


game::Game::Game() 
     :    window(500, 500, "Gyo Fish"), 
          debugWindow(300,150, "DebugState")
{
     
}

void game::Game::Run()
{
     LoadConfig();
     InitDebugFonts();
     InitDebugWindow();
     InitOverWorld(); 
     InitMiniGame();
     

     

     float fps = 1.0f / 60.0f;
     sf::Time elapsed;
     while (window.IsOpen())
     {
          elapsed += clock.Restart();
          std::vector<sf::Event> events;
          PollEvents(events);
          Input(events);

          while(elapsed.asSeconds() > fps)
          {

               Update(fps);
               DebugUpdate();
               elapsed -= sf::Time(sf::seconds(fps));
          }
          

          DebugRender();
          Render();
     }
}

void game::Game::LoadConfig()
{
     FishGameApp_ConfigHandler configHandler;
     configHandler.ReadConfigFile();
     std::string configVal;
     if (!configHandler.FindConfig("DEFAULT_WINDOW_WIDTH", configVal))
     {
          std::cout << "Config Not Loaded" << std::endl;
     }
     std::cout << configVal << std::endl;
}

void game::Game::InitOverWorld()
{
     activeQuadrant = OverworldQuadrant(RandomInt(0, 3));
     darkenedWaterOverlayShape.setFillColor(DEFAULT_DARKENED_WATER_COLOR);
     darkenedWaterOverlayShape.setSize(sf::Vector2f(overworldQuadrantVec[activeQuadrant].width, overworldQuadrantVec[activeQuadrant].height));
     darkenedWaterOverlayShape.setPosition(sf::Vector2f(overworldQuadrantVec[activeQuadrant].left, overworldQuadrantVec[activeQuadrant].top));

     if (!isImagesLoaded)
     {
          core::ImageFileReader imgReader;

          if (!imgReader.ReadImageFile("pond.png", pondTexture))
          {

          }
     }
     pondSprite.setTexture(pondTexture);
     pondSprite.setScale(DEFAULT_POND_SPRITE_SCALE);
     pondSprite.setPosition(window.GetSize().x / 2.0f - (pondTexture.getSize().x * pondSprite.getScale().x)/ 2.0f, window.GetSize().y / 2.0f - (pondTexture.getSize().y * pondSprite.getScale().y) / 2.0f);

     //overworld_drawables.push_back(pondSprite);

}

void game::Game::InitMiniGame()
{
     

     playerMarker.setSize(DEFAULT_PLAYER_MARKER_SIZE);
     playerMarker.setPosition(DEFAULT_PLAYER_STARTING_POS);
     playerMarker.setFillColor(DEFAULT_PLAYER_MARKER_COLOR);

     timerMarker.setSize(DEFAULT_TIMER_MARKER_SIZE);
     timerMarker.setPosition(DEFAULT_TIMER_STARTING_POS);
     timerMarker.setFillColor(DEFAULT_TIMER_MARKER_COLOR);

     timerShrinkingMarker.setSize(DEFAULT_SHRINKING_TIMER_MARKER_SIZE);
     timerShrinkingMarker.setPosition(DEFAULT_SHRINKING_TIMER_STARTING_POS);
     timerShrinkingMarker.setFillColor(DEFAULT_SHRINKING_TIMER_MARKER_COLOR);
     timerMarker.rotate(180.0f);

     if (!isImagesLoaded)
     {
          core::ImageFileReader imgReader;

          if (!imgReader.ReadImageFile("minigame.png", miniGameTexture))
          {

          }
     }
     miniGameSprite.setTexture(miniGameTexture);
     miniGameSprite.setScale(DEFAULT_MINIGAME_SPRITE_SCALE);
     miniGameSprite.setPosition(window.GetSize().x / 2.0f - (miniGameTexture.getSize().x * miniGameSprite.getScale().x) / 2.0f, window.GetSize().y / 2.0f - (miniGameTexture.getSize().y * miniGameSprite.getScale().y) / 2.0f);


     if (!isImagesLoaded)
     {
          core::ImageFileReader imgReader;

          if (!imgReader.ReadImageFile("fishMarker.png", fishMarkerTexture))
          {

          }
     }
     fishMarkerSprite.setTexture(fishMarkerTexture);
     fishMarkerSprite.setScale(DEFAULT_MINIGAME_SPRITE_SCALE);
     fishMarkerSprite.setPosition(/*window.GetSize().x / 2.0f - (fishMarkerTexture.getSize().x * fishMarkerSprite.getScale().x) / 2.0f*/150.0f, window.GetSize().y / 2.0f - (fishMarkerTexture.getSize().y * fishMarkerSprite.getScale().y) / 2.0f);

     currDirection = FishDirection(RandomInt(UP, DOWN));
     int directionFactor = 0;

     switch (currDirection)
     {
     case game::Game::UP:
          directionFactor = -1;
          break;
     case game::Game::STILL:
          directionFactor = 0;
          break;
     case game::Game::DOWN:
          directionFactor = 1;
          break;
     default:
          break;
     }



     currFishSpeed = RandomInt(minFishSpeed, maxFishSpeed);
     timeRemaining = DEFAULT_MINI_GAME_START_TIME;


     
}

void game::Game::InitDebugFonts()
{
     if (!DEFAULT_FONT.loadFromFile(DEFAULT_FONT_FILEPATH))
     {
          // error...
     }

     mousePosText.setFont(DEFAULT_FONT);
     mousePosText.setCharacterSize(DEFAULT_CHAR_SIZE);
     mousePosText.setFillColor(DEFAULT_COLOR);
     mousePosText.setStyle(DEFAULT_TEXT_STYLE);
     mousePosText.setString(MOUSE_POS_MSG);

     timerText.setFont(DEFAULT_FONT);
     timerText.setCharacterSize(DEFAULT_CHAR_SIZE);
     timerText.setFillColor(DEFAULT_COLOR);
     timerText.setStyle(DEFAULT_TEXT_STYLE);
     timerText.setString(TIMER_COUNTER_MSG);
     timerText.setPosition(0.0f, DEFAULT_CHAR_SIZE);
}

void game::Game::InitDebugWindow()
{
     debugWindow.SetPosition(sf::Vector2i(window.GetPosition().x - 400, window.GetPosition().y));

}

void game::Game::Input(std::vector<sf::Event>& events)
{
     for (sf::Event& i : events)
     {
          switch (i.type)
          {
          case sf::Event::Closed:
               OnClose(i);
               break;
          case sf::Event::Resized:
               OnResized(i);
               break;
          case sf::Event::EventType::LostFocus:
               OnLostFocus(i);
               break;
          case sf::Event::EventType::GainedFocus:
               OnGainedFocus(i);
               break;
          case sf::Event::EventType::KeyPressed:
               OnKeyPressed(i);
               break;
          case sf::Event::EventType::KeyReleased:
               OnKeyReleased(i);
               break;
          case sf::Event::EventType::MouseButtonPressed:
               OnMouseButtonPressed(i);
               break;
          case sf::Event::EventType::MouseButtonReleased:
               OnMouseButtonReleased(i);
               break;
          case sf::Event::EventType::MouseMoved:
               OnMouseMoved(i);
               break;
          case sf::Event::EventType::MouseEntered:
               OnMouseEntered(i);
               break;
          case sf::Event::EventType::MouseLeft:
               OnMouseLeft(i);
               break;
          default:
               break;
          }
     }
}

void game::Game::Update(float dt)
{
     //darkenedWaterOverlayShape.setPosition(overworldQuadrantVec[activeQuadrant].left, overworldQuadrantVec[activeQuadrant].top);
     
     if (currState == GameState::OVER_WORLD)
     {
          dtPerPoolAccumulator += sf::Time(sf::seconds(dt));
          if (dtPerPoolAccumulator.asSeconds() >= dtPerDarkenedPool.asSeconds())
          {
               dtPerPoolAccumulator -= dtPerDarkenedPool;

               activeQuadrant = OverworldQuadrant(RandomInt(0, 3));

               darkenedWaterOverlayShape.setPosition(overworldQuadrantVec[activeQuadrant].left, overworldQuadrantVec[activeQuadrant].top);

          }
     }
    
     
     if (currState == GameState::MINI_GAME)
     {
          
          timeRemaining -= sf::Time(sf::seconds(dt));
          
         /* if (timeRemaining.asSeconds() <= 0.0f)
          {
               WonLost();
          } */ 

          UpdateShrinkingTimerMarker(dt);
          UpdateFishMarker(dt);
          UpdatePlayerMarker(dt);   

     }     

     
     
}

void game::Game::Render()
{
     window.Clear();

     if (currState == GameState::OVER_WORLD)
     {
          window.Draw(pondSprite);
          window.Draw(darkenedWaterOverlayShape);

          
     }
     if (currState == GameState::MINI_GAME)
     {
          window.Draw(miniGameSprite);
          window.Draw(fishMarkerSprite);
          window.Draw(playerMarker);
          window.Draw(timerMarker);
          window.Draw(timerShrinkingMarker);
         
     }
     window.Display();
}

void game::Game::UpdatePlayerMarker(float dt)
{
     if(currMarkerDirection == MarkerDirection::UP_DIR)
          playerMarker.move(0.0f, DEFAULT_MARKER_VELOCITY.x * dt * currMarkerDirection);
     if (currMarkerDirection == MarkerDirection::DOWN_DIR)
          playerMarker.move(0.0f, DEFAULT_MARKER_VELOCITY.y * dt * currMarkerDirection);

     if (playerMarker.getPosition().y <= MAX_UPWARD_MARKER_POS.y) SetPlayerAtMaxPosition();
     if (playerMarker.getPosition().y >= MAX_DOWNWARD_MARKER_POS.y) SetPlayerAtMinPosition();
}

void game::Game::UpdateFishMarker(float dt)
{
     dtPerSpeedChangeAcuumulator += sf::Time(sf::seconds(dt));
     dtPerDirectionChangeAcuumulator += sf::Time(sf::seconds(dt));

     if (dtPerSpeedChangeAcuumulator >= dtPerSpeedChange)
     {
          dtPerSpeedChangeAcuumulator -= dtPerSpeedChange;
          currFishSpeed = RandomInt(minFishSpeed, maxFishSpeed);
     }

     if (dtPerDirectionChangeAcuumulator >= dtPerDirectionChange)
     {
          dtPerDirectionChangeAcuumulator -= dtPerDirectionChange;
          currDirection = FishDirection(RandomInt(FishDirection::UP, FishDirection::DOWN));
     }
     fishMarkerSprite.move(sf::Vector2f{ 0.0f, currDirection * currFishSpeed * dt });

}

void game::Game::UpdateShrinkingTimerMarker(float dt)
{
     float newSize = timerShrinkingMarker.getSize().y - ( dt *  DEFAULT_SHRINKING_TIMER_MARKER_VELOCITY.y);
     timerShrinkingMarker.setSize(sf::Vector2f(timerShrinkingMarker.getSize().x, newSize));
}

void game::Game::DebugRender()
{
     debugWindow.Clear();
     debugWindow.Draw(mousePosText);
     debugWindow.Draw(timerText);
     debugWindow.Display();
}
void game::Game::PollEvents(std::vector<sf::Event>& eventVec)
{
     sf::Event event;
     while (window.PollEvent(event))
     {
          eventVec.push_back(event);
     }
}

void game::Game::PollDebugEvents()
{
     sf::Event event;
     while (debugWindow.PollEvent(event))
     {
          if (event.type == sf::Event::Closed)
          {
               debugWindow.Close();
          }
     }
}

void game::Game::DebugUpdate()
{
     std::string temp{ MOUSE_POS_MSG };
     mousePosText.setString(std::format("Cursor: ({}, {})", sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y));
     timerText.setString(std::format("Sec.Rem: {}", timeRemaining.asSeconds()));
     
}

void game::Game::OnClose(sf::Event& event)
{
     window.Close();
}

void game::Game::OnResized(sf::Event& event)
{
         
}

void game::Game::OnLostFocus(sf::Event& event)
{
}

void game::Game::OnGainedFocus(sf::Event& event)
{
}

void game::Game::OnKeyPressed(sf::Event& event)
{
}

void game::Game::OnKeyReleased(sf::Event& event)
{
     switch (currState)
     {
     case game::OVER_WORLD:
         if (event.key.code == sf::Keyboard::Key::M)
          {
              FishOn();
                      
          }
          
         
               
          break;
     case game::MINI_GAME:
          if (event.key.code == sf::Keyboard::Key::M)
          {
               WonLost();

          }
          break;
     default:
          break;
     }
}

void game::Game::OnMouseButtonPressed(sf::Event& event)
{ 
     switch (currState)
     {
     case game::OVER_WORLD:
          OnOverWorld_MousePressed(event);
          break;
     case game::MINI_GAME:
          OnMiniGame_MousePressed(event);
          break;
     default:
          break;
     }
}

void game::Game::OnMouseButtonReleased(sf::Event& event)
{
     switch (currState)
     {
     case game::OVER_WORLD:
          OnOverWorld_MouseReleased(event);
          break;
     case game::MINI_GAME:
          OnMiniGame_MouseReleased(event);
          break;
     default:
          break;
     }
}

void game::Game::OnMouseMoved(sf::Event& event)
{
     switch (currState)
     {
     case game::OVER_WORLD:
          //std::cout << "Mouse at: " << sf::Mouse::getPosition(window).x << ", " << sf::Mouse::getPosition(window).y << std::endl;
          break;
     case game::MINI_GAME:
          //std::cout << "Mouse at: " << sf::Mouse::getPosition(window).x << ", " << sf::Mouse::getPosition(window).y << std::endl;
          break;
     default:
          break;
     }
}

void game::Game::OnMouseEntered(sf::Event& event)
{
     switch (currState)
     {
     case game::OVER_WORLD:
          break;
     case game::MINI_GAME:
          break;
     default:
          break;
     }
}

void game::Game::OnMouseLeft(sf::Event& event)
{
     switch (currState)
     {
     case game::OVER_WORLD:
          break;
     case game::MINI_GAME:
          break;
     default:
          break;
     }
}

void game::Game::OnOverWorld_MousePressed(sf::Event& event)
{
     if (darkenedWaterOverlayShape.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window))))
     {
          FishOn();
     }
}

void game::Game::OnOverWorld_MouseReleased(sf::Event& event)
{
}

void game::Game::OnMiniGame_MousePressed(sf::Event& event)
{
     if (event.mouseButton.button == sf::Mouse::Button::Left)
     {
               currMarkerDirection = MarkerDirection::UP_DIR;
         
          
     }
}

void game::Game::OnMiniGame_MouseReleased(sf::Event& event)
{
     if (event.mouseButton.button == sf::Mouse::Button::Left)
     {
    
               currMarkerDirection = MarkerDirection::DOWN_DIR;
          
     }
}

void game::Game::SetPlayerAtMaxPosition()
{
     playerMarker.setPosition(MAX_UPWARD_MARKER_POS);
}

void game::Game::SetPlayerAtMinPosition()
{
     playerMarker.setPosition(MAX_DOWNWARD_MARKER_POS);
}


