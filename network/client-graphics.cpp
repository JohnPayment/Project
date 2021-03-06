#include <cstdio> // std
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fontconfig/fontconfig.h>

#include <SFML/Graphics.hpp>

#include "types.h"

using namespace std;

enum {
    COLUMN_LEFT,
    COLUMN_MIDDLE,
    COLUMN_RIGHT
};

void update (sf::RenderWindow& window, sf::Clock& sclock);

sf::Font MyFont;
extern vector<player_matchmaking_t> players;
vector<sf::RectangleShape> shapes;
vector<sf::Text>  lables;

void add_slot ( int column, int slot, player_matchmaking_t& pmt) {
    sf::Text Text(pmt.name, MyFont, 20);
    sf::RectangleShape shape(sf::Vector2f(200, 30));
    shape.setFillColor(pmt.ready ? sf::Color(0,255,0) : sf::Color(128,128,128));
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2.0f);

    int x = 0;
    if (column == COLUMN_LEFT)
        x = 10;
    else if (column == COLUMN_MIDDLE)
        x = (800/2) - (200/2) - 5;
    else if (column == COLUMN_RIGHT)
        x = 800 - 200 - 10;

    shape.setPosition(x, 10 + (50 * slot));
    Text.setPosition (x, 10 + (50 * slot));
    shapes.push_back(shape);
    lables.push_back(Text);
}

void add_button () {
    sf::Text readytext("Ready", MyFont, 20);
    sf::RectangleShape shape(sf::Vector2f(200, 30));
    shape.setFillColor(sf::Color(255,0,0));
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(1.0f);
    shape.setPosition((800/2) - (200/2) - 5, 400);
    readytext.setPosition((800/2) - 50, 400);
    shapes.push_back(shape);
    lables.push_back(readytext);
}

void * init (void * nothing) {
    int sockfd = (long) nothing;
    sf::Clock sclock;
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

    // Pick default system font with font config.
    FcChar8 * path;
    FcResult result;
    FcPattern* pat = FcPatternCreate();
    //pat = FcNameParse ((FcChar8 *) ""); //specify font family?
    FcConfigSubstitute (0, pat, FcMatchPattern);
    FcDefaultSubstitute (pat);
    FcPattern * match =  FcFontMatch(NULL, pat, &result);
    match = FcFontMatch (0, pat, &result);
    if(FcPatternGetString(match, FC_FILE, 0, &path) == FcResultMatch)
        printf("path to arial %s\n", path);

    if (!MyFont.loadFromFile((char*)path)) {
        printf("error loading font\n");
    }

    player_matchmaking_t empty = {{0, 0}, {0}, 0, 0, 0, 0};
    strcpy(empty.name, "Empty");
    for (int i = 0; i < 5; ++i) {
        add_slot(COLUMN_RIGHT, i, empty);
        add_slot(COLUMN_LEFT, i, empty);
    }
    add_button();

    while (window.isOpen()) {
        update(window, sclock);
    }
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    return NULL;
}


void draw_slot (sf::RenderWindow& rw, int column, int slot, player_matchmaking_t& pmt) {
    sf::Text Text(pmt.name, MyFont, 20);
    sf::RectangleShape shape(sf::Vector2f(200, 30));
    shape.setFillColor(pmt.ready ? sf::Color(0,255,0) : sf::Color(128,128,128));
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2.0f);

    int x = 0;
    if (column == COLUMN_LEFT)
        x = 10;
    else if (column == COLUMN_MIDDLE)
        x = (800/2) - (200/2) - 5;
    else if (column == COLUMN_RIGHT)
        x = 800 - 200 - 10;

    shape.setPosition(x, 10 + (50 * slot));
    Text.setPosition (x, 10 + (50 * slot));
    rw.draw(shape);
    rw.draw(Text);
}

void draw_chat (sf::RenderWindow& window) {
    sf::RectangleShape shape(sf::Vector2f(700, 100));
    shape.setFillColor(sf::Color(229,229,229));
    shape.setOutlineColor(sf::Color(89,89,89));
    shape.setOutlineThickness(1.0f);
    shape.setPosition(50, 450);
    window.draw(shape);
}

void win_proc(int mesg, int id) {
    cout << "Got " << mesg << " on " << id << endl;
}

void update (sf::RenderWindow& window, sf::Clock& sclock) {
    sf::Time elapsed = sclock.restart();
    ((void) elapsed); // silence warning for now.
    
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        else if (event.type == sf::Event::MouseButtonPressed){
            sf::Vector2f mouse = sf::Vector2f(sf::Mouse::getPosition(window));
            for (size_t i = 0; i < shapes.size(); ++i) {
                if (shapes[i].getGlobalBounds().contains(mouse)) {
                    win_proc(1, i);
                }
            }
        }
    }

    sf::Vector2f mouse = sf::Vector2f(sf::Mouse::getPosition(window));
    for (size_t i = 0; i < shapes.size(); ++i) {
        if (shapes[i].getGlobalBounds().contains(mouse)) {
            shapes[i].setOutlineColor(sf::Color(255, 0,0));
        } else {
            shapes[i].setOutlineColor(sf::Color::White);
        }
    }

    window.clear();

    for (size_t i = 0; i < shapes.size(); ++i) {
        window.draw(shapes[i]);
        window.draw(lables[i]);
    }
    
    for (size_t i = 0; i < players.size(); ++i) {
        draw_slot(window, COLUMN_MIDDLE, i, players[i]);
    }

    draw_chat(window);

    window.display();
}