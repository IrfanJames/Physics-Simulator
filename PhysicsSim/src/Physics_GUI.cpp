
/*
	* Colapse all Scopes - it'll be easier
	* Ctrl + M + A in Visual Studio
*/

#include "LOG.hpp"
#include "Resource_Manager.hpp"

#include "Physics_GUI.hpp"


namespace CircuitGUI {

	// Constants
	const int gap = 15;
	const sf::Vector2f zero(0, 0);
	const sf::Color normalCompColor(230, 230, 230);

	bool Occupied = false;
	bool darkLightMode = false;
	sf::Color tempDimColor(60, 60, 60/*150, 150, 150*/);

	// Textures
	std::vector<Resource> Resource_Images;
	Resource Resource_Logo;
	Resource Resource_Font;
	//void loadTextures()

	sf::Event evnt;
	sf::RenderWindow app;
	//renderWinInit();
	sf::View view;
	//viewInit();
	sf::Font font;


	/*Cursor*/
	sf::Vector2f mouseHold;
	sf::Vector2f mouseOffSet;
	time_t click = clock(); // Time passed since Click
	bool Click(int Sensitivity = 3) {
		return (
			((float)clock() - (float)click) < 100) &&
			!!(!sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
			abs(mouseHold.x - (float)sf::Mouse::getPosition(app).x) <= Sensitivity &&
			abs(mouseHold.y - (float)sf::Mouse::getPosition(app).y) <= Sensitivity );
	}
	sf::Vector2f cursorInSim() {
		return app.mapPixelToCoords(sf::Mouse::getPosition(app));
	}
	sf::Vector2f onScreen(float X, float Y) {
		return sf::Vector2f(view.getCenter().x - view.getSize().x / 2 + X, view.getCenter().y - view.getSize().y / 2 + Y);
		//return app.mapPixelToCoords(sf::Vector2i((int)X, (int)Y));
	}
	float trim(float num, int wrt) {
		return num - (int)num % wrt;
	}
	sf::Vector2f trim(sf::Vector2f vec, int wrt) {
		vec.y = trim(vec.y, wrt);
		vec.x = trim(vec.x, wrt);

		return vec;
	}


	// Warning: Clears the vec
	void str_to_vecInt(const std::string& str, std::vector<int>& vec)
	{
		bool negative = 0, numStarted = 0;
		vec.clear();
		vec.reserve(23); // HardCode

		for (int c = 0, x = 0, temp = 0; c < str.size(); c++) {
			temp = (int)str[c];
			char chrecter = temp;

			if ('0' <= temp && temp <= '9') {
				numStarted = 1;
				x *= 10;
				x += temp - (int)'0';
			}
			else {
				if (temp == '-') negative = 1;
				else if (numStarted) {
					if (negative) x *= -1;
					vec.emplace_back(x);
					negative = 0;
					x = 0;
					numStarted = 0;
				}
			}

			if (numStarted == true && c == str.size() - 1)
			{
				if (negative) x *= -1;
				vec.emplace_back(x);
			}
		}
	}


	// Grid
	int virtualBoarder = 80;
	std::vector<sf::RectangleShape> vLines;
	std::vector<sf::RectangleShape> hLines;
	sf::Color gridColor(100, 105, 110, 20);
	sf::Color backColor(23, 24, 25);
	void colorGrid()
	{
		if (darkLightMode)
			gridColor = { 212, 232, 247, 50 };
		else
			gridColor = { 100, 105, 110, 20 }; // R,G,B,a
	}
	void colorBackground()
	{
		if (darkLightMode)
			backColor = { 36, 133, 202 };
		else
			backColor = { 23, 24, 25 }; // R,G,B
	}
	void drawGrid()
	{
		for (auto& rectangle : vLines)
			app.draw(rectangle);

		for (auto& rectangle : hLines)
			app.draw(rectangle);
	}
	//void initializeGrid()


	// Drag
	bool dragBool = 0;
	sf::Vector2f viewPos(0,0);
	sf::Vector2f ver(0,0);
	sf::Vector2f hor(0,0);
	int verBrightCount = 5, horBrightCount = 5;
	void iniDrag() {
		mouseHold = (sf::Vector2f)sf::Mouse::getPosition(app);
		viewPos = view.getCenter();
		ver = vLines[0].getPosition();
		hor = hLines[0].getPosition();
	}
	void Drag() {
		
		view.setCenter( viewPos + mouseHold - (sf::Vector2f)sf::Mouse::getPosition(app) );

		float newVerY = ver.y + mouseHold.y - (float)sf::Mouse::getPosition(app).y;
		float newHorX = hor.x + mouseHold.x - (float)sf::Mouse::getPosition(app).x;


		float verBrightX = vLines[verBrightCount].getPosition().x;
		for (int c = 0; c < vLines.size(); c++) {
			vLines[c].setPosition(trim(newHorX) + c * gap, newVerY);
		}
		float horBrightY = hLines[horBrightCount].getPosition().y;
		for (int c = 0; c < hLines.size(); c++) {
			hLines[c].setPosition(newHorX, trim(newVerY) + c * gap);
		}

		verBrightX -= vLines[verBrightCount].getPosition().x;
		if (verBrightX > gap * 0.9) verBrightCount--; else if (verBrightX < -gap * 0.9) verBrightCount++;
		//verBrightCount = wxyz + (int)((mouseX - (float)Mouse::getPosition(app).x) + (int)horX % gap) / gap;
		//verBrightCount = wxyz + (int)((mouseX - (float)Mouse::getPosition(app).x) + (-virtualBoarder - trim(horX,gap))) / gap;
		//verBrightCount = wxyz + (int)((mouseX - (float)Mouse::getPosition(app).x) + horX - (horX / (float)gap)*gap) / gap;
		//verBrightCount = wxyz + (int)((mouseX - (float)Mouse::getPosition(app).x + (-virtualBoarder + viewX - W / 2) - verX) / gap);
		//verBrightCount = wxyz + (int)((mouseX - (float)Mouse::getPosition(app).x) - (int)horX / gap) / gap;
		//cout << "\n" << mouseX - (float)Mouse::getPosition(app).x << ", " << + (-virtualBoarder + viewX - W / 2) - verX << ", " << verBrightCount;

		verBrightCount = abs((verBrightCount < 1) * (5 + verBrightCount % 5) + (1 <= verBrightCount) * ((verBrightCount - 1) % 5 + 1)) % 6;
		//cout << "\n" << verBrightCount << "\n";


		horBrightY -= hLines[horBrightCount].getPosition().y;
		if (horBrightY > gap * 0.9) horBrightCount--; else if (horBrightY < -gap * 0.9) horBrightCount++;
		horBrightCount = abs((horBrightCount < 1) * (5 + horBrightCount % 5) + (1 <= horBrightCount) * ((horBrightCount - 1) % 5 + 1)) % 6;
		//cout << "\n" << horBrightCount << "\n";
	}
	void colorBrightLineGrid() {
		for (int c = 0; c < vLines.size(); c++) {
			gridColor.a = 20 + ((c + verBrightCount) % 5 == 0) * 15;
			vLines[c].setFillColor(gridColor);
		}
		for (int c = 0; c < hLines.size(); c++) {
			gridColor.a = 20 + ((c + horBrightCount) % 5 == 0) * 15;
			hLines[c].setFillColor(gridColor);
		}
	}


	// Vectors
	std::vector<Entity> entities;
	void drawEntities()
	{
		for (auto& entity : entities)
			entity.draw(app);
	}


	void addToVector(int integer, std::vector<int>& vec)
	{
		//if (std::binary_search(vec.begin(), vec.end(), integer) == false)
		{
			auto it = std::lower_bound(vec.begin(), vec.end(), integer);
			if (it == vec.end()) // Not Found
				vec.emplace_back(integer);
			else {
				//if (vec[it - vec.begin() + 1] != integer)
				//vec.emplace(it, integer);

				int insert_pos = it - vec.begin();
				if (insert_pos <= vec.size() - !vec.empty() && vec[insert_pos] != integer)
					vec.insert(vec.begin() + insert_pos, integer);
			}
		}
	}

	void updateThemeColors()
	{
		if (darkLightMode) tempDimColor = { 60, 60, 60 };
		else tempDimColor = { 150, 150, 150 };

		colorGrid();
		colorBackground();
		colorBrightLineGrid();
	}
	void initializeGUI()
	{
		//initializeRenderWindow();
		{
			int W = sf::VideoMode::getDesktopMode().width * 0.85;
			int H = W * 9 / 16;

			app.create(sf::VideoMode((unsigned int)W, (unsigned int)H), "CircuitSim", sf::Style::Default, sf::ContextSettings(0, 0, 8));
			app.clear(backColor);
			app.display();
			W = app.getSize().x; H = app.getSize().y;
			
			app.setPosition(sf::Vector2i(sf::VideoMode::getDesktopMode().width / 2 - W / 2, sf::VideoMode::getDesktopMode().height / 2 - H / 2 - 50));
			app.setKeyRepeatEnabled(false);
			//app.setVerticalSyncEnabled(true);

#ifdef _DEBUG
			std::ifstream window_size("temp_files/win_size.txt");
			if (window_size.good())
			{
				int arr[4] = { W, H, app.getPosition().x, app.getPosition().y };
				window_size >> arr[0] >> arr[1] >> arr[2] >> arr[3];
				window_size.close();

				app.setSize(sf::Vector2u(arr[0], arr[1]));
				app.setPosition(sf::Vector2i(arr[2], arr[3]));
			}
#endif
		}

		//themeColors
		updateThemeColors();
		
		//setFont
		{
			//ResourceID_Fonts.SetAll(IDR_FONT1, "FONT");
			//Entity::setFont(ResourceID_Fonts.GetResource().ptr, ResourceID_Fonts.GetResource().size_bytes);
			font.loadFromFile("assets/Fonts/CalibriL_1.ttf");

			//ResourceID_Fonts.SetAll(IDR_FONT1, "FONT");
			//Item::setFont(ResourceID_Fonts.GetResource().ptr, ResourceID_Fonts.GetResource().size_bytes);
		}

		//viewInit();
		{
			view.setCenter(sf::Vector2f(app.getSize().x / 2, app.getSize().y / 2));
			view.setSize((sf::Vector2f)app.getSize());
			//sf::View view(sf::Vector2f(app.getSize().x / 2, app.getSize().y / 2), sf::Vector2f(app.getSize().x * 2, app.getSize().y * 2));
			//sf::View view(sf::Vector2f(app.getSize().x / 2, app.getSize().y / 2), (sf::Vector2f)app.getSize());
		}

		//setIcon
		{
			Resource_Logo.SetAll(IDR_LOGO, "BMP");

			if (Resource_Logo.GetResource().ptr != nullptr) {
				sf::Image logo; // No need to keep it alive after setIcon()
				if (logo.loadFromMemory(Resource_Logo.GetResource().ptr, Resource_Logo.GetResource().size_bytes))
					app.setIcon(logo.getSize().x, logo.getSize().y, logo.getPixelsPtr());
			}
		}

		//initializeGrid();
		{
			colorGrid();
			colorBackground();

			int W = app.getSize().x;
			int H = app.getSize().y;

			int a = (int)((sf::VideoMode::getDesktopMode().width - W) / 2);
			int b = (int)((sf::VideoMode::getDesktopMode().height - H) / 2);
			virtualBoarder = (a >= b) ? a + 5 * gap : b + 5 * gap;

			vLines.reserve((W + 2 * virtualBoarder) / gap + 2);
			sf::Vector2f tempVect(1, H + 2 * virtualBoarder);
			for (int c = 0; c <= (W + 2 * virtualBoarder) / gap; c++) {
				vLines.emplace_back(tempVect);
			}
			for (int c = 0; c < vLines.size(); c++) {
				vLines[c].setPosition(-virtualBoarder - (int)(-virtualBoarder) % gap + c * gap, -virtualBoarder);
				//gridColor.a = 20 + (c % 5 == 0) * 15;
				//vLines[c].setFillColor(gridColor);
			}

			hLines.reserve((H + 2 * virtualBoarder) / gap + 2);
			tempVect.x = W + 2 * virtualBoarder; tempVect.y = 1;
			for (int c = 0; c <= (H + 2 * virtualBoarder) / gap; c++) {
				hLines.emplace_back(tempVect);
			}
			for (int c = 0; c < hLines.size(); c++) {
				hLines[c].setPosition(-virtualBoarder, -virtualBoarder - (int)(-virtualBoarder) % gap + c * gap);
				//gridColor.a = 20 + (c % 5 == 0) * 15;
				//hLines[c].setFillColor(gridColor);
			}

			colorBrightLineGrid();

			/*{
			colorGrid();
			colorBackground();

			vLines.reserve((W + 2 * virtualBoarder) / gap + 2);
			sf::Vector2f tempVect(1, H + 2 * virtualBoarder);
			for (int c = 0; c <= (W + 2 * virtualBoarder) / gap; c++) {
				vLines.emplace_back(tempVect);
			}
			for (int c = 0; c < vLines.size(); c++) {
				vLines[c].setPosition(-virtualBoarder - (int)(-virtualBoarder) % gap + c * gap, -virtualBoarder);
				//gridColor.a = 20 + (c % 5 == 0) * 15;
				//vLines[c].setFillColor(gridColor);
			}

			hLines.reserve((H + 2 * virtualBoarder) / gap + 2);
			tempVect.x = W + 2 * virtualBoarder; tempVect.y = 1;
			for (int c = 0; c <= (H + 2 * virtualBoarder) / gap; c++) {
				hLines.emplace_back(tempVect);
			}
			for (int c = 0; c < hLines.size(); c++) {
				hLines[c].setPosition(-virtualBoarder, -virtualBoarder - (int)(-virtualBoarder) % gap + c * gap);
				//gridColor.a = 20 + (c % 5 == 0) * 15;
				//hLines[c].setFillColor(gridColor);
			}

			colorBrightLineGrid();
		}*/
		}

		// Vectors
		{
			entities.reserve(10);
			for (int i = 1; i <= 8; i++)
				CircuitGUI::entities.emplace_back(sf::Vector2f(i * 90, i * 90), sf::Color(i * 40, i * 72, i * 786));
		}

	}



	namespace Options
	{
		void Open(const std::string& filepath) {

			std::ifstream input(filepath);
			/*int fileNo = 0;
			std::string fileDir = "Saved-Projects/Project-", fileType = ".txt";
			std::ifstream input(fileDir + std::to_string(fileNo) + fileType);
			while (input.good()) {
				input.close();
				input.open(fileDir + std::to_string(++fileNo) + fileType);
			}
			input.open(fileDir + std::to_string(fileNo - 1) + fileType);*/



			input.close();


		}

		void Save(const std::string& file) {

			//std::cout << "\nCtrl + S\n";
			//ShellExecute(NULL, NULL, L"Saved-Projects", NULL, NULL, SW_SHOWNORMAL);

			std::ofstream output;

			//int fileNo = 0;
			//std::string fileDir = "Saved-Projects/Project-", fileType = ".txt";
			//std::ifstream test(fileDir + std::to_string(fileNo) + fileType);
			//while (test.good()) {
			//	test.close();
			//	test.open(fileDir + std::to_string(++fileNo) + fileType);
			//}
			//output.open(fileDir + std::to_string(fileNo) + fileType);

			//output.open(SaveFile("text file (*.txt)\0*.txt\0"));

			//if (file.empty()) return;

			output.open(file);

			std::string tempStr;
			
			output << tempStr;

			output.close();

			WCHAR hello[260] = { 0 };
			for (int i = 0; i < 260 && i < file.length(); i++) hello[i] = file[i];

			ShellExecute(NULL, NULL, hello, NULL, NULL, SW_SHOWNORMAL);
		}

		void SaveAsImage(const std::string& filepath) {

			/*try
			{
				LOG(filepath);
				int len;
				int slength = (int)filepath.length() + 1;
				len = MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), slength, 0, 0);
				wchar_t* buf = new wchar_t[len];
				MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), slength, buf, len);
				std::wstring r(buf);
				delete[] buf;
				const wchar_t* widecstr = r.c_str();

				//ShellExecute(NULL, NULL, L"Saved-Images", NULL, NULL, SW_SHOWNORMAL);
				ShellExecute(NULL, NULL, widecstr, NULL, NULL, SW_SHOWNORMAL);
			}
			catch (const std::exception&)
			{
				LOG("Shell connot open floder  \"Saved - Images\"");
			}*/

			sf::Image image;
			

			image.saveToFile(filepath);

			//int picNo = 0;
			//std::string picDir = "Saved-Images/Untitled-", picType = ".png";
			//sf::Image test;
			//while (test.loadFromFile(picDir + std::to_string(picNo++) + picType)) {
			//	;
			//	LOG( "\n" << picNo);
			//}
			//image.saveToFile(picDir + std::to_string(picNo - 1) + picType);


			WCHAR Filepath_w[260] = { 0 }; // HardCode (size of arr)
			for (int i = 0; i < 260 && i < filepath.length(); i++) Filepath_w[i] = filepath[i];
			ShellExecute(NULL, NULL, Filepath_w, NULL, NULL, SW_SHOWNORMAL);
		}
	}

}