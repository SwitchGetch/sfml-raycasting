#include "Includes.h"
#include "Define.h"
#include "Enums.h"
#include "Classes.h"
#include "Functions.h"


int main()
{
	high_resolution_clock::time_point start;
	high_resolution_clock::time_point end;

	RenderWindow window(VideoMode(1000, 750), "");
	window.setFramerateLimit(60);

	RectangleShape object0(Vector2f(100, 100));
	object0.setPosition(Vector2f(400, 100));
	object0.setFillColor(Color::Red);

	RectangleShape object1(Vector2f(100, 100));
	object1.setPosition(Vector2f(300, 400));
	object1.setFillColor(Color::Green);

	RectangleShape object2(Vector2f(100, 100));
	object2.setPosition(Vector2f(600, 500));
	object2.setFillColor(Color::Blue);

	RectangleShape object3(Vector2f(100, 100));
	object3.setPosition(Vector2f(700, 200));
	object3.setFillColor(Color::Yellow);

	vector<RectangleShape> map;

	map.push_back(object0);
	map.push_back(object1);
	map.push_back(object2);
	map.push_back(object3);


	RectangleShape square(Vector2f(100, 100));
	square.setFillColor(Color(120, 120, 120));

	Vector2f position(square.getSize().x / 2, square.getSize().y / 2);
	ViewingAngle angle(0, 90, 0.5, 1250, 1);

	Font font;
	Text text;
	bool stats_output = false;
	bool view_3d = false;

	font.loadFromFile("arial.ttf");
	text.setFont(font);
	text.setFillColor(Color::Green);


	while (window.isOpen())
	{
		start = high_resolution_clock::now();

		Event event;
		if (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			if (event.type == Event::KeyPressed)
			{
				switch (event.key.scancode)
				{
				case Keyboard::Scancode::F:

					stats_output = (stats_output ? false : true);

					break;

				case Keyboard::Scancode::D:

					view_3d = (view_3d ? false : true);

					break;
				}
			}
		}


		if (Keyboard::isKeyPressed(Keyboard::Up))
		{
			position += Vector2f(cos(angle.average() * DegToRad), sin(angle.average() * DegToRad));
		}
		if (Keyboard::isKeyPressed(Keyboard::Down))
		{
			position -= Vector2f(cos(angle.average() * DegToRad), sin(angle.average() * DegToRad));
		}
		if (Keyboard::isKeyPressed(Keyboard::Left))
		{
			angle.min -= angle.rotate_angle;
			angle.max -= angle.rotate_angle;
		}
		if (Keyboard::isKeyPressed(Keyboard::Right))
		{
			angle.min += angle.rotate_angle;
			angle.max += angle.rotate_angle;
		}


		window.clear();


		VertexArray line(Lines, 2);
		vector<Collision> collisions;

		line[0].position = position;

		line[0].color = Color(200, 200, 200);
		line[1].color = Color(200, 200, 200);

		for (double a = angle.min; a <= angle.max; a += angle.step)
		{
			bool b = false;

			for (int i = 1; i <= angle.range; i++)
			{
				if (b) break;

				Vector2f current = Vector2f(i * cos(a * DegToRad) + position.x, i * sin(a * DegToRad) + position.y);

				for (int j = 0; j < map.size(); j++)
				{
					if (
						(current.x >= map[j].getPosition().x && current.y >= map[j].getPosition().y &&
						current.x <= map[j].getPosition().x + map[j].getSize().x && current.y <= map[j].getPosition().y + map[j].getSize().y) ||
						(current.x <= 0 || current.x >= window.getSize().x || current.y <= 0 || current.y >= window.getSize().y)
						)
					{
						collisions.push_back(Collision(current, CollisionOption::Object));

						b = true;

						break;
					}
					else if (i == (int)angle.range)
					{
						collisions.push_back(Collision(current, CollisionOption::OutOfRange));

						b = true;

						break;
					}
				}
			}
		}


		if (view_3d)
		{
			RectangleShape sky(Vector2f(window.getSize().x, window.getSize().y / 2)), ground(Vector2f(window.getSize().x, window.getSize().y / 2));

			sky.setFillColor(Color(59, 219, 247));

			ground.setPosition(Vector2f(0, window.getSize().y / 2));
			ground.setFillColor(Color(16, 163, 21));

			window.draw(sky);
			window.draw(ground);

			int ray_count = collisions.size();

			double width = window.getSize().x / (double)ray_count;
			double height = 50000;

			for (int i = 0; i < ray_count; i++)
			{
				if (collisions[i].collision_option == CollisionOption::Object)
				{
					double length = height / distance(line[0].position, collisions[i].position);

					RectangleShape object_line(Vector2f(width, length));

					object_line.setPosition(Vector2f(i * width, (window.getSize().y - length) / 2));
					object_line.setFillColor(Color(120, 120, 120));

					window.draw(object_line);
				}
			}
		}
		else
		{
			VertexArray triangle(Triangles, 3);

			triangle[0].position = line[0].position;

			triangle[0].color = Color(200, 200, 200);
			triangle[1].color = Color(200, 200, 200);
			triangle[2].color = Color(200, 200, 200);

			for (int i = 0; i < collisions.size() - 1; i++)
			{
				triangle[1].position = collisions[i].position;
				triangle[2].position = collisions[i + 1].position;

				window.draw(triangle);
			}

			square.setPosition(Vector2f(line[0].position.x - square.getSize().x / 2, line[0].position.y - square.getSize().y / 2));

			window.draw(square);

			for (int i = 0; i < map.size(); i++) window.draw(map[i]);
		}


		if (stats_output)
		{
			string text_str =
				"position:\nx: " + to_string(position.x) +
				"\ny: " + to_string(position.y) +
				"\nviewing angle:\nmin: " + to_string(angle.min) +
				"\nmax: " + to_string(angle.max) +
				"\nstep: " + to_string(angle.step) +
				"\nrange: " + to_string(angle.range) +
				"\naverage: " + to_string(angle.average()) +
				"\n ray count: " + to_string(collisions.size());

			text.setString(text_str);

			window.draw(text);
		}


		window.display();


		end = high_resolution_clock::now();

		double fps = 1e9f / duration_cast<chrono::nanoseconds>(end - start).count();

		string title = "FPS " + to_string(fps);
		window.setTitle(title.c_str());
	}
}

//#include <iostream>
//#include <SFML/Graphics.hpp>
//#include <cmath>
//using namespace std;
//using namespace sf;
//
//#define RadToDeg 180 / 3.1415926
//#define DegToRad 3.1415926 / 180
//
//double length(Vector2f first, Vector2f second)
//{
//	return sqrt(pow(first.x - second.x, 2) + pow(first.y - second.y, 2));
//}
//
//void oneLine(RenderWindow& window, RectangleShape& line, RectangleShape object)
//{
//	line.setPosition(Vector2f(Mouse::getPosition().x - window.getPosition().x, Mouse::getPosition().y - window.getPosition().y));
//
//	line.setSize(Vector2f(length(line.getPosition(), object.getPosition()), 1));
//
//	line.setRotation(RadToDeg * acos(abs(line.getPosition().x - object.getPosition().x) / line.getSize().x));
//}
//
//void allLines(RenderWindow& window, RectangleShape& line1, RectangleShape& line2, RectangleShape& line3, RectangleShape& line4, RectangleShape object)
//{
//	line1.setPosition((Vector2f)Mouse::getPosition(window));
//	line2.setPosition((Vector2f)Mouse::getPosition(window));
//	line3.setPosition((Vector2f)Mouse::getPosition(window));
//	line4.setPosition((Vector2f)Mouse::getPosition(window));
//
//	line1.setSize(Vector2f(length(line1.getPosition(), object.getPosition()), 2));
//	line2.setSize(Vector2f(length(line2.getPosition(), Vector2f(object.getPosition().x + object.getSize().x, object.getPosition().y)), 2));
//	line3.setSize(Vector2f(length(line3.getPosition(), Vector2f(object.getPosition().x, object.getPosition().y + object.getSize().y)), 2));
//	line4.setSize(Vector2f(length(line4.getPosition(), Vector2f(object.getPosition().x + object.getSize().x, object.getPosition().y + object.getSize().y)), 2));
//
//	Vector2f side1 = Vector2f(line1.getPosition().x - object.getPosition().x, line1.getPosition().y - object.getPosition().y);
//	Vector2f side2 = Vector2f(line2.getPosition().x - (object.getPosition().x + object.getSize().x), line2.getPosition().y - object.getPosition().y);
//	Vector2f side3 = Vector2f(line3.getPosition().x - object.getPosition().x, line3.getPosition().y - (object.getPosition().y + object.getSize().y));
//	Vector2f side4 = Vector2f(line4.getPosition().x - (object.getPosition().x + object.getSize().x), line4.getPosition().y - (object.getPosition().y + object.getSize().y));
//
//	double tan1 = side1.y / side1.x;
//	double tan2 = side2.y / side2.x;
//	double tan3 = side3.y / side3.x;
//	double tan4 = side4.y / side4.x;
//
//	double angle1 = RadToDeg * atan(tan1);
//	double angle2 = RadToDeg * atan(tan2);
//	double angle3 = RadToDeg * atan(tan3);
//	double angle4 = RadToDeg * atan(tan4);
//
//	if (line1.getPosition().x >= object.getPosition().x) angle1 += 180;
//	if (line2.getPosition().x >= object.getPosition().x + object.getSize().x) angle2 += 180;
//	if (line3.getPosition().x >= object.getPosition().x) angle3 += 180;
//	if (line4.getPosition().x >= object.getPosition().x + object.getSize().x) angle4 += 180;
//
//	line1.setRotation(angle1);
//	line2.setRotation(angle2);
//	line3.setRotation(angle3);
//	line4.setRotation(angle4);
//
//	/*line1.setSize(Vector2f(line1.getSize().x + window.getSize().x, 2));
//	line2.setSize(Vector2f(line2.getSize().x + window.getSize().x, 2));
//	line3.setSize(Vector2f(line3.getSize().x + window.getSize().x, 2));
//	line4.setSize(Vector2f(line4.getSize().x + window.getSize().x, 2));*/
//}
//
//int main()
//{
//	int winX = 1000, winY = 750;
//
//	RenderWindow window(VideoMode(winX, winY), "My window");
//	window.setFramerateLimit(60);
//
//	RectangleShape object(Vector2f(190, 140));
//	object.setPosition(Vector2f((winX - object.getSize().x) / 2, (winY - object.getSize().y) / 2));
//	object.setFillColor(Color(194, 194, 194));
//
//	Vector2f speed_direction(1, 1);
//	double speed_module = 1;
//
//	RectangleShape line1, line2, line3, line4;
//
//	line1.setFillColor(Color::Red);
//	line2.setFillColor(Color::Green);
//	line3.setFillColor(Color::Blue);
//	line4.setFillColor(Color::Yellow);
//
//	Font font;
//	Text text;
//	bool stats_output = false;
//
//	font.loadFromFile("arial.ttf");
//
//	text.setFont(font);
//	text.setFillColor(Color::White);
//
//
//	while (window.isOpen())
//	{
//		sf::Event event;
//		while (window.pollEvent(event))
//		{
//			if (event.type == sf::Event::Closed)
//			{
//				window.close();
//			}
//
//			if (event.type == Event::KeyPressed)
//			{
//				switch (event.key.scancode)
//				{
//				case Keyboard::Scancode::D:
//
//					stats_output = (stats_output ? false : true);
//
//					break;
//				}
//			}
//		}
//
//		window.clear(sf::Color::Black);
//
//
//		allLines(window, line1, line2, line3, line4, object);
//
//		window.draw(object);
//
//		window.draw(line1);
//		window.draw(line2);
//		window.draw(line3);
//		window.draw(line4);
//
//		if (stats_output)
//		{
//			string text_str =
//				"mouse:\nx: " + to_string(Mouse::getPosition().x) + "\n" +
//				"y: " + to_string(Mouse::getPosition().y) + "\n" +
//				"object:\nx: " + to_string(object.getPosition().x) + "\n" +
//				"y: " + to_string(object.getPosition().y) + "\n" +
//				"speed direction:\nx: " + to_string(speed_direction.x) + "\n" +
//				"y: " + to_string(speed_direction.y) + "\n" +
//				"speed module: " + to_string(speed_module) + "\n" +
//				"object points:\n";
//
//			for (int i = 0; i < object.getPointCount(); i++)
//			{
//				text_str += to_string(object.getPosition().x + object.getPoint(i).x) + " " + to_string(object.getPosition().y + object.getPoint(i).y) + "\n";
//			}
//
//			text.setString(text_str);
//
//			window.draw(text);
//		}
//
//		window.display();
//
//
//		for (int i = 0; i < (int)speed_module; i++)
//		{
//			object.move(speed_direction);
//
//			if (
//				object.getPosition().x <= 0 || object.getPosition().x + object.getSize().x >= winX ||
//				object.getPosition().y <= 0 || object.getPosition().y + object.getSize().y >= winY
//				)
//			{
//				if (object.getPosition().x <= 0 || object.getPosition().x + object.getSize().x >= winX) speed_direction.x *= -1;
//
//				if (object.getPosition().y <= 0 || object.getPosition().y + object.getSize().y >= winY) speed_direction.y *= -1;
//
//				speed_module += 0.1;
//			}
//		}
//	}
//}


//#include <iostream>
//#include <thread>
//#include <chrono>
//#include <random>
//#include <string>
//#include <vector>
//#include <SFML/Graphics.hpp>
//#include <SFML/System.hpp>
//#include <SFML/Window.hpp>
//#include <SFML/Audio.hpp>
//using namespace std;
//using namespace sf;
//
//int main()
//{
//    srand(time(NULL));
//
//
//    RenderWindow window(sf::VideoMode(1000, 500), "My window");
//
//
//    vector<Font> fonts;
//    Text text;
//
//    for (int i = 0; i < 7; i++)
//    {
//        Font font;
//
//        font.loadFromFile("Fonts/" + to_string(i) + ".ttf");
//
//        fonts.push_back(font);
//    }
//
//    text.setString("PRESS SPACE TO WINDOWS");
//    text.setPosition(Vector2f(0, 0));
//    text.setCharacterSize(50);
//
//
//    int current_frame = 0;
//
//    vector<Texture> textures;
//    RectangleShape rectangle;
//
//    for (int i = 0; i < 2; i++)
//    {
//        Texture texture;
//
//        texture.loadFromFile("Frames/" + to_string(i) + ".png");
//
//        textures.push_back(texture);
//    }
//
//    rectangle.setPosition(Vector2f(375, 125));
//    rectangle.setSize(Vector2f(250, 250));
//
//
//    vector<SoundBuffer> sound_buffers;
//    Sound sound;
//
//    for (int i = 0; i < 7; i++)
//    {
//        SoundBuffer sound_buffer;
//
//        sound_buffer.loadFromFile("Sounds/" + to_string(i) + ".ogv");
//
//        sound_buffers.push_back(sound_buffer);
//    }
//
//
//    while (window.isOpen())
//    {
//        Event event;
//        while (window.pollEvent(event))
//        {
//            if (event.type == Event::Closed)
//            {
//                window.close();
//            }
//
//            if (event.type == Event::KeyPressed)
//            {
//                switch (event.key.scancode)
//                {
//                case Keyboard::Scancode::Space:
//
//                    sound.setBuffer(sound_buffers[rand() % 7]);
//
//                    sound.play();
//
//                    break;
//                }
//            }
//        }
//
//        window.clear(Color(204, 14, 0));
//
//
//        rectangle.setTexture(&textures[current_frame]);
//
//        window.draw(rectangle);
//
//
//        text.setFont(fonts[rand() % 7]);
//        text.setFillColor(Color(rand() % 256, rand() % 256, rand() % 256));
//
//        window.draw(text);
//
//
//        window.display();
//
//        current_frame++;
//        current_frame %= textures.size();
//
//        this_thread::sleep_for(chrono::milliseconds(250));
//    }
//}