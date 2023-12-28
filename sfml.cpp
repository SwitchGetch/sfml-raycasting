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
