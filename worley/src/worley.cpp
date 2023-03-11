// Markus Buchholz, 2023

#include <stdafx.hpp>
#include "imgui_helper.hpp"
#include <tuple>
#include <vector>
#include <math.h>
#include <random>
#include <algorithm>

//---------------------------------------------------------------

int w = 1000;
int h = 1000;

int size = 1;
int rows = 500;
int cols = 500;

//---------------------------------------------------------------

struct Noise
{

	ImVec2 p0;
	ImVec2 p1;
	int color;

	Noise(ImVec2 p0i, ImVec2 p1i, int c) : p0(p0i), p1(p1i), color(c){};
	Noise(ImVec2 p0i, ImVec2 p1i) : p0(p0i), p1(p1i){};
};

//---------------------------------------------------------------

struct Img
{

	float x;
	float y;
	int c;
};

//---------------------------------------------------------------

int generateRandom()
{

	std::random_device engine;
	std::mt19937 generator(engine());
	std::uniform_int_distribution<int> distrib(1, 255);
	return distrib(generator);
}
//---------------------------------------------------------------

float generateRandomDot()
{

	std::random_device device;
	std::mt19937 engine(device());
	std::uniform_real_distribution<float> dist(0.0, (float)499.0 * size); //size

	return dist(engine);
}
//---------------------------------------------------------------

int main(int argc, char const *argv[])
{

	std::string title = "Worley Noice";
	initImgui(w, h, title);

	bool show_demo_window = true;
	bool show_another_window = false;
	//ImVec4 clear_color = ImVec4(89.0f / 255.0, 88.0f / 255.0, 87.0f / 255.0, 1.00f);
	ImVec4 clear_color = ImVec4(24.0f / 255.0, 24.0f / 255.0, 82.0f / 255.0, 1.00f);

	int ii = 0;
	bool flag = true;


	while (!glfwWindowShouldClose(window) && flag == true)
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		static int dots_nr = 15;

		ImGuiWindowFlags window_flags = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("Simulation", nullptr, window_flags);
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		std::vector<ImVec2> dots;

		for (int ii = 0; ii < dots_nr; ii++)
		{
			dots.push_back({generateRandomDot(), generateRandomDot()});
		}

		std::vector<Noise> noise_img;
		std::vector<std::vector<Noise>> worley;

		for (int ii = 0; ii < cols; ii++)
		{
			std::vector<Noise> worley_i;
			for (int jj = 0; jj < rows; jj++)
			{

				Noise w({(float)ii * size, (float)jj * size}, {(float)ii * size + size, (float)jj * size + 2 * size}, generateRandom());
				worley_i.emplace_back(w);
			}

			worley.emplace_back(worley_i);
		}

		int radious = 2.0;
		for (auto &ii : dots)
		{

			draw_list->AddCircleFilled(ii, radious, IM_COL32(255, 0, 0, 255));
		}

		for (int ii = 0; ii < cols; ii++)
		{
			for (int jj = 0; jj < rows; jj++)
			{

				std::vector<std::tuple<int, int, float>> distances;
				for (auto &dot : dots)
				{

					auto d = std::sqrt(std::pow(worley[ii][jj].p0.x - dot.x, 2) + std::pow(worley[ii][jj].p0.y - dot.y, 2));
					distances.push_back(std::make_tuple(ii, jj, d));
				}

				auto dist_min = [=](std::tuple<int, int, float> a, std::tuple<int, int, float> b)
				{
					return std::get<2>(a) < std::get<2>(b) ? true : false;
				};

				std::sort(distances.begin(), distances.end(), dist_min);
				draw_list->AddRectFilled(worley[ii][jj].p0, worley[ii][jj].p1, IM_COL32(0, 0, 255, std::get<2>(distances[0])));

			}
		}

		ImGui::PushItemWidth(300);
		ImGui::SliderInt("dots", &dots_nr, 1, 100);
		ImGui::PopItemWidth();
		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	termImgui();
	return 0;
}
