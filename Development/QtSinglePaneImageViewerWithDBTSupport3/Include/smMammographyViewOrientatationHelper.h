#pragma once

#ifndef SMMAMMOGRAPHYVIEWORIENTATATIONHELPER_H
#define SMMAMMOGRAPHYVIEWORIENTATATIONHELPER_H

/////////////////////////////////////////////////////////////////////////////////////////

#include <memory>
#include <array>

class smDicomReader;

/////////////////////////////////////////////////////////////////////////////////////////

class smMammographyViewOrientatationHelper
{
public:
	smMammographyViewOrientatationHelper(std::shared_ptr<smDicomReader> pReader);
	~smMammographyViewOrientatationHelper();

	using AxesDirectionCosines = std::array<double, 9>;
	using Vector1x3 = std::array<double, 3>;

public:
	bool							Update();
	bool							hasValidData() const;
	bool							hasIdentityAxesDirectionCosines() const;
	bool							isAxesDirectionCosinesFilpZ() const;
	const AxesDirectionCosines&		getAxesDirectionCosines() const;
	int								getProperViewConventionForImage() const;
	void							setAxesDirectionCosines(const AxesDirectionCosines& cosines);
	bool							isFFDM() const;
	bool							isDBT_BTO() const;
	//bool							isDBT() const;
public:
	static bool	isIdentityCosines(const AxesDirectionCosines& directionCosines);
	
private:
	class Private;
	std::unique_ptr<Private> m_pPrivate;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // SMMAMMOGRAPHYVIEWORIENTATATIONHELPER_H
