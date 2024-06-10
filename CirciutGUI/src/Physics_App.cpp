
/*
	* Colapse all Scopes - it'll be easier
	* Ctrl + M + A in Visual Studio
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <direct.h>
#include <chrono>
#include <future>

//#include <thread>
//#include <sstream>
//#include <string>

#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/Graphics.hpp"

#include "LOG.hpp"
#include "Timer.hpp"
#include "Physics_Windows_Stuff.hpp"

#include "Physics.hpp"
#include "Physics_GUI.hpp"
#include "Physics_App.hpp"


App::App(const std::vector<std::string>& filepaths)
{
	srand(time(NULL));

	CircuitGUI::initializeGUI();

	ImGui::SFML::Init(CircuitGUI::app);
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	//Resource my_pic(IDR_CAP, "BMP");
	//Resource my_font(IDR_FONT2, "FONT");
	//my_font.GetInfo();


#ifdef _DEBUG
	// For Opening Where Left
	CircuitGUI::Options::Open("temp_files/last.txt");
#endif

	//////////////// argv //////////////////
	if (filepaths.empty() == false)
	{
		CircuitGUI::Options::Open(filepaths.back());
	}

}

App::~App()
{
	//ImGui
	ImGui::SFML::Shutdown();

#ifdef _DEBUG
	CircuitGUI::Options::Save("temp_files/last.txt");
#else
	LOG::log_file.close();
#endif
}

void App::Run()
{
	//////////////// Main Loop ////////////////////
	while (CircuitGUI::app.isOpen() && End == false)
	{
		Events();

		ImGUI();

		Options();

		Threads();

		Update();

		Render();

		EndFrame();
	}

}



void App::Events()
{
	while (CircuitGUI::app.pollEvent(CircuitGUI::evnt)) {
		using namespace CircuitGUI;

		// ImGui
		ImGui::SFML::ProcessEvent(evnt);

		if (evnt.type == evnt.Closed) {

			std::ofstream window_size("temp_files/win_size.txt");
			window_size << CircuitGUI::app.getSize().x << "\t";
			window_size << CircuitGUI::app.getSize().y << "\t";
			window_size << CircuitGUI::app.getPosition().x << "\t";
			window_size << CircuitGUI::app.getPosition().y << "\t";
			window_size.close();

			app.close(); End = 1;
		}

		if (evnt.type == evnt.MouseLeft) { cursorInWin = 0; }

		if (evnt.type == evnt.MouseEntered) { cursorInWin = 1; }

		if (evnt.type == evnt.Resized) {
			view = sf::View(sf::FloatRect(
				(int)view.getCenter().x - (int)(evnt.size.width / 2),
				(int)view.getCenter().y - (int)(evnt.size.height / 2),
				(int)evnt.size.width,
				(int)evnt.size.height));
			//CircuitGUI::view.setSize((int)evnt.size.width, (int)evnt.size.height);
			//CircuitGUI::view.setCenter((int)CircuitGUI::view.getCenter().x, (int)CircuitGUI::view.getCenter().y);


			std::ofstream window_size("temp_files/win_size.txt");
			window_size << CircuitGUI::app.getSize().x << "\t";
			window_size << CircuitGUI::app.getSize().y << "\t";
			window_size << CircuitGUI::app.getPosition().x << "\t";
			window_size << CircuitGUI::app.getPosition().y << "\t";
			window_size.close();
		}

		if (evnt.type == evnt.MouseButtonPressed)
		{
			// Selecting Item
			if (evnt.mouseButton.button == sf::Mouse::Left)
			{
				if(sf::Mouse::getPosition(app).x < app.getSize().x - ImGuiWinLeft)
				{
					selectedItem = -1;

					for (int i = 0; i < CircuitGUI::entities.size(); i++)
						if (CircuitGUI::entities[i].sprite.getGlobalBounds().contains(CircuitGUI::cursorInSim()))
						{
							selectedItem = i;
							break;
						}
				}
			}

			if (evnt.mouseButton.button == sf::Mouse::Middle) {
				dragBool = true;
				CircuitGUI::iniDrag();
			}
		}

		if (evnt.type == evnt.MouseButtonReleased)
		{
			if (evnt.mouseButton.button == sf::Mouse::Middle) {
				dragBool = false;
			}
		}

		if (evnt.type == evnt.KeyPressed)
		{
			if (evnt.key.code == sf::Keyboard::Escape) {
				app.close();
				End = 1;
				continue;
			}

			if (evnt.key.code == sf::Keyboard::Space) {
				PausePhysics = !PausePhysics;
			}

			// Ctrl
			if (evnt.key.control) {
				if (evnt.key.code == sf::Keyboard::O) {

					std::string filepath = OpenFileDialog("text file (*.txt)\0*.txt\0");

					if (!filepath.empty())
						CircuitGUI::Options::Open(filepath);

				}
				if (evnt.key.code == sf::Keyboard::S) {
					;
					//_mkdir("Saved-Images");		// Alredy Exists(-1) else Created(0)
					//_mkdir("Saved-Projects");	// Alredy Exists(-1) else Created(0)
				}
				if (evnt.key.code == sf::Keyboard::S && evnt.key.shift) {

					_mkdir("Saved-Images");		// Alredy Exists(-1) else Created(0)
					_mkdir("Saved-Projects");	// Alredy Exists(-1) else Created(0)
					std::string filepath = SaveFileDialog("Project file (*.TXT)\0*.TXT\0PNG (*.PNG)\0*.PNG\0");//JPEG (*.JPG)\0*.JPG\0

					if (!filepath.empty()) {

						if (filepath.back() == 'T')
							CircuitGUI::Options::Save(filepath);

						if (filepath.back() == 'G')
							futures.emplace_back(std::async(std::launch::async, CircuitGUI::Options::SaveAsImage, filepath));

					}

				}
			}
		}

	}


}

void App::ImGUI() {
	using namespace CircuitGUI;
	ImGuiInFocus = false;

	ImGui::SFML::Update(app, deltaClock.restart());
	/*
	ImGui::Begin("Frist ImGui Win");
	ImGui::Text("My Project will be on Steroids");
	ImGui::Checkbox("Draw Circle", &DrawCircle);
	ImGui::SliderFloat("Radius", &t_radius, 0, 200);
	ImGui::SliderInt("Sides", &t_vertices, 3, 35);
	ImGui::ColorEdit3("Color", t_Colors);
	ImGui::End();

	testCircle.setRadius(t_radius);
	testCircle.setOrigin(testCircle.getRadius(), testCircle.getRadius());
	testCircle.setPointCount(t_vertices);
	testCircle.setFillColor(sf::Color((int)(t_Colors[0] * 255), (int)(t_Colors[1] * 255), (int)(t_Colors[2] * 255)));//*/

	if (ImGui::BeginMainMenuBar())
	{
		if (/*ImGui::IsWindowFocused() || */ ImGui::IsWindowHovered()) {
			//LOG("\nMainBar");
			ImGuiInFocus = true;
		}
		if (ImGui::BeginMenu("File"))
		{
			ImGuiInFocus = true;

			if (ImGui::MenuItem("New", "Ctrl + N")) {
				// stimuliDisplay = 1;
			}
			if (ImGui::MenuItem("Open...", "Ctrl + O")) {

				std::string filepath = OpenFileDialog("text file (*.txt)\0*.txt\0");

				if (!filepath.empty())
					CircuitGUI::Options::Open(filepath);
			}
			if (ImGui::MenuItem("Save", "Ctrl + S")) {
				;
				//_mkdir("Saved-Images"); // Alredy Exists(-1) else Created(0)
				//_mkdir("Saved-Projects"); // Alredy Exists(-1) else Created(0)
			}
			if (ImGui::MenuItem("Save As...", "Ctrl + Shift + S")) {

				_mkdir("Saved-Images"); // Alredy Exists(-1) else Created(0)
				_mkdir("Saved-Projects"); // Alredy Exists(-1) else Created(0)

				std::string filepath = SaveFileDialog("Project file (*.TXT)\0*.TXT\0PNG (*.PNG)\0*.PNG\0");//JPEG (*.JPG)\0*.JPG\0

				if (!filepath.empty()) {

					if (filepath.back() == 'T')
						CircuitGUI::Options::Save(filepath);

					if (filepath.back() == 'G')
						CircuitGUI::Options::SaveAsImage(filepath);
						//futures.emplace_back(std::async(std::launch::async, CircuitGUI::Options::SaveAsImage, filepath));
				}

			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "Esc")) { CircuitGUI::app.close(); End = 1; /*continue;*/ }

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Options"))
		{
			ImGuiInFocus = true;

			if (ImGui::BeginMenu("Handdrawn Icons"))
			{
				ImGui::Text("Coming Soon...");
				ImGui::EndMenu();
			}
			ImGui::Separator();
			//ImGui::MenuItem("Show QuadTree", "", &visible_QuadTree);
			if (ImGui::BeginMenu("Theme")) {

				if (ImGui::MenuItem("Dark")) {

					darkLightMode = false;

					updateThemeColors();
				}
				if (ImGui::MenuItem("Light")) {

					darkLightMode = true;

					updateThemeColors();
				}

				ImGui::EndMenu();
			}


			//asdf
			/*ImGui::Separator();
			if (ImGui::MenuItem("Game")) {
				stimuliDisplay = 1; stimuliEndNodes = 1; //cout << "17";
				PlayMode = !PlayMode;

				CircuitGUI::comp.clear();
				CircuitGUI::virSerial.clear();
				CircuitGUI::virSprite.clear();
				CircuitGUI::virSerialShift.clear();

				//asdfwires.clear();

				if (PlayMode) {
					CircuitGUI::Options::openf("Saved-Projects\\Maze.TXT");
					//ShellExecute(0, 0, L"https://www.youtube.com/watch?v=6cRctjPRv6M", 0, 0, SW_SHOW);
				}
				else { wireBool = 0; PlayRot = 0; }
			}*/

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGuiInFocus = true;

			if (ImGui::BeginMenu("Controls"))
			{
				if (ImGui::BeginTable("table_context_menu_2", 2))
				{
					static const std::string opt[11][2] = { //HardCode
						{ "Drag View", "Press Scroll-Wheel + Drag" },
						{ "Selection Mode", "Click + Hold + Drag" },
						{ "Select All",		"Ctrl + A" },
						{ "Rotate",			"Ctrl + R" },
						{ "Delete",			"Del" },
						{ "Copy",			"Ctrl + C" },
						{ "Paste",			"Ctrl + V" },
						{ "Open",			"Ctrl + O" },
						{ "Save As",		"Ctrl + Shift + S" },
						{ "Escape",			"Esc" },
						{ "New Components",	"Hover Left" }
					};

					ImGui::TableSetupColumn("Option");
					ImGui::TableSetupColumn("Keys");

					ImGui::TableHeadersRow();
					ImGui::TableNextRow();

					for (int row = 0; row < 11; row++) //HardCode
					{
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s", opt[row][0].c_str());
						ImGui::SameLine();

						ImGui::TableSetColumnIndex(1);
						//ImGui::Text(opt[row][1].c_str());

						bool sameLine = false;
						std::string Text = opt[row][1] + " ", temp = "";
						for (int i = 0; i < Text.length(); i++) {

							if (('0' <= Text[i] && Text[i] <= '9') || ('a' <= Text[i] && Text[i] <= 'z') || ('A' <= Text[i] && Text[i] <= 'Z')) {
								temp += Text[i];
							}
							else if (Text[i] == '+') {
								if (sameLine) ImGui::SameLine();
								ImGui::Text("+");
								sameLine = true;
							}
							else {
								if (temp.size()) {
									if (sameLine) ImGui::SameLine();
									ImGui::SmallButton(temp.c_str());
									temp.clear();
									sameLine = true;
								}
							}

						}

						ImGui::TableNextRow();
					}
					ImGui::EndTable();
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Contacts"))
			{
				static std::string links[2][2] = // HardCode
				{
					{ "Copy Github", "https://github.com/IrfanJames/CircuitSimulator_SFML" },
					{ "Copy Email ", "irfanjamespak@gmail.com" }
				};

				for (int i = 0; i < 2; i++) {
					if (ImGui::Button(links[i][0].c_str()))
						sf::Clipboard::setString(links[i][1]);
					ImGui::SameLine();
					ImGui::Text("%s", links[i][1].c_str());
				}

				ImGui::EndMenu();
			}

			/*ImGui::Separator();

			if (ImGui::BeginMenu("Options"))
			{
				static bool enabled = true;
				ImGui::MenuItem("Enabled", "", &enabled);
				ImGui::BeginChild("child", ImVec2(0, 60), true);
				for (int i = 0; i < 10; i++)
					ImGui::Text("Scrolling Text %d", i);
				ImGui::EndChild();
				static float f = 0.5f;
				static int n = 0;
				ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
				ImGui::InputFloat("Input", &f, 0.1f);
				ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
				ImGui::EndMenu();
			}*/

			ImGui::EndMenu();
		}

		/*ImGui::Begin("Right-Click");
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::Selectable("Apple")) LOG("\nApple");
			if (ImGui::Selectable("Banana")) LOG("\nBanana");
			ImGui::EndPopup();
		}
		ImGui::End;

		if (ImGui::BeginPopup("Set Value")) {

			static float value = 0;
			ImGui::DragFloat("Value", &value);

			comp[virSerial.front()].voltage = value;

		}
		ImGui::EndPopup();*/


		//if (ImGui::SmallButton("AD")) {
		//	LOG("\nAds");
		//}
		//
		//if (ImGui::Button("ad")) {
		//	LOG("\nAds");
		//}

		ImGui::EndMainMenuBar();
	}

	//Docking
	; {
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::Begin("Docking Space", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground);

		ImGuiID dockspaceID = ImGui::GetID("MyDockspace");
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
		//ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::End();
	}

	if (ImGui::Begin("Window-A##ID"))
	{
		if (ImGui::IsWindowFocused() || ImGui::IsWindowHovered())
			ImGuiInFocus = true;

		if (selectedItem != -1)
		{
			// Transforms
			if (ImGui::CollapsingHeader("Transforms", ImGuiTreeNodeFlags_DefaultOpen)) {
				int bt = 50;
				auto& item = CircuitGUI::entities[selectedItem];

				static float rot = 0;
				rot = item.Rotation;
				ImGui::Button("Rot "); ImGui::SameLine();
				ImGui::SetNextItemWidth(70); ImGui::DragFloat(".", &rot); item.Rotation = rot;

				ImGui::Button("Size"); ImGui::SameLine();
				ImGui::Button("X"); ImGui::SameLine();
				ImGui::SetNextItemWidth(70); ImGui::DragFloat(",", &item.Size.x); ImGui::SameLine();
				ImGui::Button("Y"); ImGui::SameLine();
				ImGui::SetNextItemWidth(70); ImGui::DragFloat(":", &item.Size.y);

				ImGui::Button("Pos "); ImGui::SameLine();
				ImGui::Button("X"); ImGui::SameLine();
				ImGui::SetNextItemWidth(70); ImGui::DragFloat("|", &item.Position.x); ImGui::SameLine();
				ImGui::Button("Y"); ImGui::SameLine();
				ImGui::SetNextItemWidth(70); ImGui::DragFloat(";", &item.Position.y);

				ImGui::Button("Vel "); ImGui::SameLine();
				ImGui::Button("X"); ImGui::SameLine();
				ImGui::SetNextItemWidth(70); ImGui::DragFloat("/", &item.Velocity.x); ImGui::SameLine();
				ImGui::Button("Y"); ImGui::SameLine();
				ImGui::SetNextItemWidth(70); ImGui::DragFloat("l", &item.Velocity.y);

				item.stimuli();
			}

			// Rigid Body
			if (ImGui::CollapsingHeader("RigidBody", ImGuiTreeNodeFlags_DefaultOpen)) {


				if (ImGui::TreeNodeEx("Mode", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
				{
					auto& type = CircuitGUI::entities[selectedItem].Rigid.BodyType;

					if (ImGui::RadioButton("Active", type == RigidBody::Active))
						type = RigidBody::Active;
					if (ImGui::RadioButton("Passive", type == RigidBody::Passive))
						type = RigidBody::Passive;
					if (ImGui::RadioButton("Not Rigid", type == RigidBody::NotRigid))
						type = RigidBody::NotRigid;

					ImGui::TreePop();
				}
				ImGui::Text("");


				if (ImGui::TreeNodeEx("Body Shape", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
				{
					auto& type = CircuitGUI::entities[selectedItem].Rigid.BodyShape;

					if (ImGui::RadioButton("Circular", type == RigidBody::Circular))
						type = RigidBody::Circular;
					if (ImGui::RadioButton("Rectangular", type == RigidBody::Rectangular))
						type = RigidBody::Rectangular;
					if (ImGui::RadioButton("ConvexShape", type == RigidBody::ConvexShape))
						type = RigidBody::ConvexShape;

					ImGui::TreePop();
				}
				ImGui::Text("");

				ImGui::Separator();
			}
		}

		ImGuiWinLeft = ImGui::GetWindowWidth();
	}
	ImGui::End();
}

void App::Options() {

	if (ImGuiInFocus) return;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		// Dragging Selected Item
		{
			if (selectedItem != -1)
			{
				CircuitGUI::entities[selectedItem].Position = CircuitGUI::cursorInSim();
				CircuitGUI::entities[selectedItem].stimuli();
			}
		}
	}

}

void App::Threads()
{
	for (int i = 0; i < futures.size(); i++) {

		std::future_status status = futures[i].wait_for(std::chrono::milliseconds(10));

		if (status == std::future_status::ready) //{
		// The future is ready (thread completed or value available)
		//	LOG("ready");
			futures.erase(futures.begin() + i--);
		//}
		//else if (status == std::future_status::timeout) {
		//	// The future is not ready within the specified duration
		//	LOG("timeout");
		//}
		//else if (status == std::future_status::deferred) {
		//	// The future is deferred (using std::promise and std::async(launch::deferred))
		//	LOG("deferred");
		//}

	}
}

void App::Update()
{
	using namespace CircuitGUI;

	// Drag
	{
		if (dragBool) {
			CircuitGUI::Drag();
			CircuitGUI::colorBrightLineGrid();
		}
	}


	if(PausePhysics)
	{
		static Timer PhysicsTimer;
		double duration = PhysicsTimer.duration() / 1000.0;
		if (0.1 < duration) duration = 0.1;

		for (auto& en : entities)
			en.update(duration);

		Physics::Update(duration);
		PhysicsTimer.reset();
	}
}

void App::Render()
{
	using namespace CircuitGUI;

	app.setView(view);

	app.clear(backColor);

	drawGrid();

	drawEntities();

	ImGui::SFML::Render(app); // Last Thing to render

	// CircuitGUI::app.display(); // Inside void App::EndFrame()
}

void App::EndFrame()
{
	CircuitGUI::app.display();


	// FPS
	{
		static Timer FPS_Timer;
		double duration = FPS_Timer.duration();
		CircuitGUI::app.setTitle("CircuitSim   " + std::to_string(duration) + " | " + std::to_string(1000 / duration));
		FPS_Timer.reset();
	}
}

