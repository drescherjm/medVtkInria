#pragma once

#ifndef MAMMOGRAPHYVIEWORIENTATATIONHELPER_H
#define MAMMOGRAPHYVIEWORIENTATATIONHELPER_H

/////////////////////////////////////////////////////////////////////////////////////////

#include <memory>
#include <array>

class DicomReader;

/////////////////////////////////////////////////////////////////////////////////////////

class MammographyViewOrientatationHelper
{
public:
	MammographyViewOrientatationHelper(std::shared_ptr<DicomReader> pReader);
	~MammographyViewOrientatationHelper();
public:
	bool					Update();
	bool					hasValidData() const;
	bool							hasIdentityAxesDirectionCosines() const;
	bool							isAxesDirectionCosinesFilpZ() const;
	const std::array<double, 9>&	getAxesDirectionCosines() const;
	int								getProperViewConventionForImage() const;
	bool							isFFDM() const;
	bool							isDBT_BTO() const;
	//bool							isDBT() const;

private:
	class Private;
	std::unique_ptr<Private> m_pPrivate;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // MAMMOGRAPHYVIEWORIENTATATIONHELPER_H
