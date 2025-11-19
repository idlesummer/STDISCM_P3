#pragma once
#include "../core/Scene.hpp"
#include "../core/AssetManager.hpp"
#include "../game/tetris/TetrisEngine.hpp"
#include "../entities/Board.hpp"
#include "../entities/Tetromino.hpp"
#include "../entities/TetrisScoreText.hpp"
#include "../entities/NextPiecePreview.hpp"
#include "../entities/HoldPiecePreview.hpp"
#include "../entities/MenuText.hpp"
#include "../entities/LoadingProgressBar.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>

using namespace std;
using namespace sf;

class TetrisScene : public Scene {
private:
    // Game engine (owns all game logic)
    TetrisEngine engine;

    // SFML rendering entities
    shared_ptr<Board> board;
    shared_ptr<Tetromino> activePiece;
    shared_ptr<TetrisScoreText> scoreDisplay;
    shared_ptr<NextPiecePreview> nextPreview;
    shared_ptr<HoldPiecePreview> holdPreview;
    shared_ptr<MenuText> titleText;
    shared_ptr<MenuText> gameOverText;
    shared_ptr<MenuText> controlsText;
    shared_ptr<LoadingProgressBar> loadingProgressBar;

    // Optional block texture
    Texture blockTexture;
    bool hasTexture;

    // SFML-specific state (not game logic)
    Time fallTimer;
    Time fallInterval;

public:
    TetrisScene()
        : engine(),
          board(),
          activePiece(),
          scoreDisplay(),
          nextPreview(),
          holdPreview(),
          titleText(),
          gameOverText(),
          controlsText(),
          loadingProgressBar(),
          blockTexture(),
          hasTexture(false),
          fallTimer(Time::Zero),
          fallInterval(seconds(1.0f)) {
    }

    void onCreate() override {
        // Initialize game engine
        this->engine.start();

        // Queue all existing textures for background loading
        auto& assetManager = AssetManager::getInstance();
        auto iconsPath = filesystem::path("assets/images/icons");

        if (filesystem::exists(iconsPath) && filesystem::is_directory(iconsPath)) {
            // Collect all PNG files
            auto pngFiles = vector<string>();
            for (const auto& entry : filesystem::directory_iterator(iconsPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".png") {
                    pngFiles.push_back(entry.path().filename().string());
                }
            }

            // Sort files to ensure consistent loading order
            sort(pngFiles.begin(), pngFiles.end());

            // Queue all found textures for loading
            for (const auto& filename : pngFiles) {
                assetManager.loadTexture(filename);
            }

            cout << "[TetrisScene] Queued " << pngFiles.size() << " textures for loading" << endl;
        } else {
            cout << "[TetrisScene] Warning: assets/images/icons directory not found" << endl;
        }

        // Try to load optional block texture
        this->hasTexture = this->blockTexture.loadFromFile("assets/images/icons/tile000.png");
        if (!this->hasTexture) {
            // Silently fall back to solid color rendering
        }

        // Create board entity (renders engine's board)
        this->board = make_shared<Board>(
            &this->engine.getBoard(),
            this->hasTexture ? &this->blockTexture : nullptr
        );
        this->addEntity(this->board);

        // Create UI
        auto text = "Arrow Keys: Move/Rotate | Space: Hard Drop | Shift: Hold";
        this->scoreDisplay = make_shared<TetrisScoreText>(Vector2f(400, 50));
        this->nextPreview = make_shared<NextPiecePreview>(Vector2f(400, 150));
        this->holdPreview = make_shared<HoldPiecePreview>(Vector2f(400, 320));
        this->titleText = make_shared<MenuText>("TETRIS", Vector2f(400, 10), 30);
        this->controlsText = make_shared<MenuText>(text, Vector2f(50, 660), 16);
        this->loadingProgressBar = make_shared<LoadingProgressBar>(Vector2f(400, 500), 200.f, 30.f);

        this->addEntity(this->scoreDisplay);
        this->addEntity(this->nextPreview);
        this->addEntity(this->holdPreview);
        this->addEntity(this->titleText);
        this->addEntity(this->controlsText);
        this->addEntity(this->loadingProgressBar);

        // Sync UI with engine state
        this->syncVisualState();
    }

    void onInput(Event& event) override {
        if (this->engine.isGameOver()) {
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter)
                this->restartGame();
            return;
        }

        if (event.type == Event::KeyPressed) {
            switch (event.key.code) {
                default: break;

                case Keyboard::Left:
                    this->engine.moveLeft(); break;

                case Keyboard::Right:
                    this->engine.moveRight(); break;

                case Keyboard::Down:
                    if (this->engine.softDrop())        // Reset fall timer on manual drop
                        this->fallTimer = Time::Zero; 
                    break;

                case Keyboard::Up:
                    this->engine.rotate(); break;

                case Keyboard::Space:
                    this->engine.hardDrop();
                    this->lockPiece();
                    break;

                case Keyboard::LShift:
                    if (this->engine.hold()) {
                        this->syncVisualState();
                        this->fallTimer = Time::Zero;
                    }
                    break;
            }
        }
    }

    void onUpdate(Time dt) override {
        // Update all entities (animations, etc.)
        this->updateEntities(dt);

        if (this->engine.isGameOver() || !this->engine.getActivePiece())
            return;

        // Update fall timer
        this->fallTimer += dt;
        if (this->fallTimer < this->fallInterval)
            return;

        this->fallTimer = Time::Zero;

        // Try to move piece down (gravity)
        if (!this->engine.softDrop())
            this->lockPiece();  // Piece can't move down - lock it
    }

    // Draw all entities (board, pieces, UI)
    void onDraw(RenderWindow& window) override {
        this->drawEntities(window);
    }

private:
    // Synchronize SFML entities with engine state
    void syncVisualState() {
        // Update next piece preview
        this->nextPreview->setNextPiece(this->engine.getNextPieceType());

        // Update hold piece preview
        this->holdPreview->setHeldPiece(this->engine.getHeldPieceType());
        this->holdPreview->setLocked(!this->engine.canHold());

        // If engine has an active piece, syn visual entity
        if (this->engine.getActivePiece()) {
            
            // Create visual entity if none exists
            if (!this->activePiece) {
                this->activePiece = make_shared<Tetromino>(
                    this->engine.getActivePiece(),
                    this->board.get(),
                    this->hasTexture ? &this->blockTexture : nullptr
                );
                this->addEntity(this->activePiece);
            } 

            // Otherwise, update existing entity
            else    
                this->activePiece->setPiece(this->engine.getActivePiece());

        // Elif engine doesn't hav active piece, remove entity
        } else if (this->activePiece) {
            // Remove active piece entity
            this->removeEntity(this->activePiece);
            this->activePiece = nullptr;
        }
    }

    void lockPiece() {
        auto linesCleared = this->engine.lockCurrentPiece();

        // Update score if lines were cleared
        if (linesCleared > 0)
            this->scoreDisplay->addLines(linesCleared);

        // Check for game over
        if (this->engine.isGameOver()) {
            this->triggerGameOver();
            return;
        }

        // Sync visual state with new piece
        this->syncVisualState();
    }

    void triggerGameOver() {
        // Remove active piece entity
        if (this->activePiece) {
            this->removeEntity(this->activePiece);
            this->activePiece = nullptr;
        }

        // Show game over text
        this->gameOverText = make_shared<MenuText>("GAME OVER", Vector2f(250, 300), 40);
        this->addEntity(this->gameOverText);

        auto scoreText = make_shared<MenuText>(
            "Final Lines: " + to_string(this->engine.getTotalLinesCleared()),
            Vector2f(250, 360), 24);
        this->addEntity(scoreText);

        auto restartText = make_shared<MenuText>(
            "Press ENTER to restart",
            Vector2f(230, 420), 20);
        this->addEntity(restartText);
    }

    void restartGame() {
        // Clear all entities
        this->clearEntities();

        // Reset engine
        this->engine.reset();

        // Reset SFML state
        this->fallTimer = Time::Zero;
        this->activePiece = nullptr;

        // Recreate everything
        this->onCreate();
    }
};
