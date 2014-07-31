#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <string>
#include <stdlib.h>
#include <sstream>

class ViewManager
{
public:

	// Classic Constructor
	ViewManager();

	// Custom Constructor
	ViewManager(std::string filePath, std::string imageSpecifier, std::string imageType, int windowWidth, int windowHeight, int numImages, double imageRatio, int xUnitDen, int yUnitDen)
	{
		m_nWindowWidth = windowWidth;
		m_nWindowHeight = windowHeight;

		/*if (windowWidth > windowHeight) {
			m_fXCounter = windowWidth / windowHeight;
			m_fYCounter = 1;
		}
		else {
			m_fXCounter = 1;
			m_fYCounter = windowHeight / windowWidth;
		}*/
		m_fXCounter = windowWidth / xUnitDen;
		m_fYCounter = windowHeight / yUnitDen;

		m_fImageCounter = imageRatio;

		for (int i = 1; i < numImages + 1; i++) {
			std::cout << "Loading image " << i << " of " << numImages << std::endl;
			std::stringstream sstrm;

			sstrm << filePath << '\\' << imageSpecifier;

			if (i < 10) {
				sstrm << "000";
			}
			else if (i < 100) {
				sstrm << "00";
			}
			else {
				sstrm << '0';
			}
			
			sstrm << i << imageType;

			sf::Texture pTexture;
			if (!pTexture.loadFromFile(sstrm.str())) {
				std::exit(EXIT_FAILURE);
			}

			m_vTextures.push_back(pTexture);
		}

		m_nOriginImage = numImages / 2;
		m_nCurrentImage = m_nOriginImage;

		m_spImageSprite.setTexture(m_vTextures.at(m_nCurrentImage));
		auto size = m_spImageSprite.getTexture()->getSize();
		m_spImageSprite.setScale((float)m_nWindowWidth / size.x, (float)m_nWindowHeight / size.y);

		m_nCircleX = m_nWindowWidth / 2;
		m_nCircleY = m_nWindowHeight / 2;

		m_csCircle.setRadius(2.0f);
		m_csCircle.setOutlineColor(sf::Color::Green);
		m_csCircle.setFillColor(sf::Color::Transparent);
		m_csCircle.setOutlineThickness(2.0f);
		m_csCircle.setPosition(m_nCircleX, m_nCircleY);
	};

	// Inline Functions
	int updateImage(int newImage)
	{
		if (floor(m_nOriginImage + (newImage * m_fImageCounter)) >= m_vTextures.size() - 1) {
			m_nCurrentImage = m_vTextures.size() - 1;
		}
		else if (floor(m_nOriginImage + (newImage * m_fImageCounter)) <= 0) {
			m_nCurrentImage = 0;
		}
		else {
			m_nCurrentImage = floor(m_nOriginImage + newImage * m_fImageCounter);
		}
		return m_nCurrentImage;
	};

	int updateCircleX(int newX) {

		double Cx_window = m_nWindowWidth / 2; // * m_fXCounter;

		int xW = floor((m_fXCounter * newX) + Cx_window);

		if (xW >= m_nWindowWidth) {
			m_nCircleX = m_nWindowWidth;
		}
		else if (xW <= 0) {
			m_nCircleX = 0;
		}
		else {
			m_nCircleX = xW;
		}

		return m_nCircleX;
	};

	int updateCircleY(int newY) {

		double Cy_window = m_nWindowHeight / 2; // * m_fYCounter;
		int yW = floor((m_fYCounter * newY) + Cy_window);

		if (yW >= m_nWindowHeight) {
			m_nCircleY = m_nWindowHeight;
		}
		else if (yW <= 0) {
			m_nCircleY = 0;
		}
		else {
			m_nCircleY = yW;
		}
		return m_nCircleY;
	};

	void resetImage(int index) {
		m_spImageSprite.setTexture(m_vTextures.at(index));
		return;
	};

	void resetCircle(int x, int y){
		m_csCircle.setPosition(x,y);
		return;
	};

	// Setters
	void setOriginImage(int newImage) {
		m_nOriginImage = updateImage(newImage);
		return;
	};

	// Constant Functions
	const int getWindowWidth() const { return m_nWindowWidth; };
	const int getWindowHeight() const { return m_nWindowHeight; };
	sf::Sprite getImageSprite() const{ return m_spImageSprite; };
	sf::CircleShape getCircle() const { return m_csCircle; };
	int getOriginImage() const { return m_nOriginImage; };

	// Classic Destructor
	~ViewManager(void){};

private:
	std::vector<sf::Texture> m_vTextures;
	sf::Sprite m_spImageSprite;
	sf::CircleShape m_csCircle;
	int m_nCurrentImage;
	int m_nOriginImage;
	double m_fImageCounter;
	int m_nWindowWidth;
	int m_nWindowHeight;
	int m_nCircleX;
	int m_nCircleY;
	double m_fXCounter;
	double m_fYCounter;
};