#include "smCasePCH.h"
#include "smMammographyViewOrientatationHelper.h"
#include "smDicomReader.h"
#include "smvtkImageView2D.h"

#include <array>
#include <vtkMatrix4x4.h>

#define DEBUG_ORIENTATION

/////////////////////////////////////////////////////////////////////////////////////////

using VTKView = smvtkImageView2D;

using DblArr9 = std::array<double, 9>;

static constexpr DblArr9 IdentityCosines{ 1,0,0,0,1,0,0,0,1 };
static constexpr DblArr9 FlipZCosines{ 1,0,0,0,1,0,0,0,-1 };


/////////////////////////////////////////////////////////////////////////////////////////

class smMammographyViewOrientatationHelper::Private
{
public:
	Private(std::shared_ptr<smDicomReader> pReader);
public:
	int getProperViewConventionForImage(std::string strLaterality, std::string strPatientOrientation,
		std::string strMQCMCode, vtkMatrix4x4* pPatientMatrix);
	static int GetCodeForDBT_LMLO_LCC(vtkMatrix4x4* pPatientMatrix, std::string strPatientOrientation,DblArr9& pArrayCosines);
	static int GetCodeForDBT_RMLO_RCC(vtkMatrix4x4* pPatientMatrix, std::string strPatientOrientation,DblArr9& pArrayCosines);
	static int GetCodeForDBT_LLM(vtkMatrix4x4* pPatientMatrix, std::string strPatientOrientation,DblArr9& pArrayCosines);
	static int GetCodeForDBT_RLM(vtkMatrix4x4* pPatientMatrix, std::string strPatientOrientation,DblArr9& pArrayCosines);
	static int GetCodeForDBT_LXCCL(vtkMatrix4x4* pPatientMatrix, std::string strPatientOrientation,DblArr9& pArrayCosines);
	static int GetCodeForDBT_RXCCL(vtkMatrix4x4* pPatientMatrix, std::string strPatientOrientation,DblArr9& pArrayCosines);
	void updateAxesDirectionCosines(std::string strLaterality, std::string strMQCMCode, vtkMatrix4x4* pPatientMatrix);
	bool isFFDM() const;
	bool isDBT_BTO() const;
public:
	std::shared_ptr<smDicomReader>	m_pReader;
	bool							m_bInitialized{ false };
	int								m_nViewConvention{ VTKView::VIEW_CONVENTION_LUNG_HFS_AXIAL_VIEW_AXIAL };
	DblArr9							m_aryAxesCosines{ IdentityCosines };
};

/////////////////////////////////////////////////////////////////////////////////////////

smMammographyViewOrientatationHelper::Private::Private(std::shared_ptr<smDicomReader> pReader) : m_pReader{ pReader }
{

}

/////////////////////////////////////////////////////////////////////////////////////////

bool smMammographyViewOrientatationHelper::Private::isFFDM() const
{
	bool retVal{ false };

	auto dicomSOPClass = m_pReader->GetSOPClassUID();

	static std::set<std::string> ffdmClasses{ "1.2.840.10008.5.1.4.1.1.1.2",
		"1.2.840.10008.5.1.4.1.1.1.2.1" };

	if (!dicomSOPClass.empty()) {
		retVal = (ffdmClasses.find(dicomSOPClass) != ffdmClasses.end());
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smMammographyViewOrientatationHelper::Private::isDBT_BTO() const
{
	bool retVal{ false };

	auto dicomSOPClass = m_pReader->GetSOPClassUID();

	if (!dicomSOPClass.empty()) {
		retVal = (dicomSOPClass == "1.2.840.10008.5.1.4.1.1.13.1.3");
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

int smMammographyViewOrientatationHelper::Private::GetCodeForDBT_LMLO_LCC(vtkMatrix4x4* pPatientMatrix, std::string strPatientOrientation, DblArr9& arrayCosines)
{
	if (pPatientMatrix) {
		auto val = pPatientMatrix->GetElement(1, 0);
		auto val1 = pPatientMatrix->GetElement(0, 1);
		if (val < 0) {
			if (val1 < 0) {
				return 3;
			}
			else {
				return 2;
			}
		}
		else if (val1 > 0) {
			// We need to flip the ZAxis
			arrayCosines[8] = -1;
			return 4;
		}
	}

	static std::unordered_set<std::string> orientationsThatFlip = { R"(P\R)", R"(P\FR)" };

	if (orientationsThatFlip.count(strPatientOrientation)) {
		arrayCosines[8] = -1;
	}
	return 4;
}

/////////////////////////////////////////////////////////////////////////////////////////

int smMammographyViewOrientatationHelper::Private::GetCodeForDBT_RMLO_RCC(vtkMatrix4x4* pPatientMatrix, std::string strPatientOrientation, DblArr9& arrayCosines)
{
	if (pPatientMatrix) {
		// Detect if we need to flip the Z axis
		auto val = pPatientMatrix->GetElement(1, 0);
		auto val1 = pPatientMatrix->GetElement(0, 1);
		if ((val1 < 0) && (val < 0)) {
			arrayCosines[8] = -1;
			return 4;
		}
	}
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////

int smMammographyViewOrientatationHelper::Private::GetCodeForDBT_LLM(vtkMatrix4x4* pPatientMatrix, std::string strPatientOrientation, DblArr9& arrayCosines)
{
	if (pPatientMatrix) {
		if (pPatientMatrix->GetElement(1, 0) > 0) {
			return 8;
		}
	}
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////

int smMammographyViewOrientatationHelper::Private::GetCodeForDBT_RLM(vtkMatrix4x4* pPatientMatrix, std::string strPatientOrientation, DblArr9& arrayCosines)
{
	return 3;
}

/////////////////////////////////////////////////////////////////////////////////////////

int smMammographyViewOrientatationHelper::Private::GetCodeForDBT_LXCCL(vtkMatrix4x4* pPatientMatrix, std::string strPatientOrientation, DblArr9& arrayCosines)
{
	return 4;
}

/////////////////////////////////////////////////////////////////////////////////////////

int smMammographyViewOrientatationHelper::Private::GetCodeForDBT_RXCCL(vtkMatrix4x4* pPatientMatrix, std::string strPatientOrientation, DblArr9& arrayCosines)
{
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////

int smMammographyViewOrientatationHelper::Private::getProperViewConventionForImage(std::string strLaterality, std::string strPatientOrientation, 
	std::string strMQCMCode, vtkMatrix4x4* pPatientMatrix)
{
	int retVal = VTKView::VIEW_CONVENTION_LUNG_HFS_AXIAL_VIEW_AXIAL;

	if (!strLaterality.empty() && !strMQCMCode.empty()) {

		std::string strCode = strLaterality[0] + strMQCMCode;

		bool bMultiframe = m_pReader->isMultiframeDicom();
		if (bMultiframe) {

			using DetectionFunction = std::function<int(vtkMatrix4x4*, std::string,DblArr9 &)>;

			static std::map<std::string, DetectionFunction> codeMap{
				{"LMLO",GetCodeForDBT_LMLO_LCC},
				{"LCC",GetCodeForDBT_LMLO_LCC},
				{"LML",GetCodeForDBT_LMLO_LCC},
				{"RMLO",GetCodeForDBT_RMLO_RCC},
				{"RCC",GetCodeForDBT_RMLO_RCC},
				{"RML",GetCodeForDBT_RMLO_RCC},
				{"LLM",GetCodeForDBT_LLM},
				{"RLM",GetCodeForDBT_RLM},
				{"LXCCL",GetCodeForDBT_LXCCL},
				{"RXCCL",GetCodeForDBT_RXCCL},
			};

			auto it = codeMap.find(strCode);
			if (it != codeMap.end()) {
				retVal = it->second(pPatientMatrix, strPatientOrientation, m_aryAxesCosines);
			}
		}
		else {
			if (isFFDM()) {
				retVal = VTKView::VIEW_CONVENTION_RADIOLOGICAL_BREAST;
			}
			else {
				auto imagesInAcquisition = m_pReader->GetImagesInAcquisition();
				if ((imagesInAcquisition) && (imagesInAcquisition.value() == 1)) {
					retVal = VTKView::VIEW_CONVENTION_RADIOLOGICAL_BREAST;
				}
			}
		}

	}

	std::cout << "Detected View Convention: " << retVal << '\n';
	if (m_aryAxesCosines == FlipZCosines) {
		std::cout << "Flip Z Axis while using convention: " << retVal << '\n';
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smMammographyViewOrientatationHelper::Private::updateAxesDirectionCosines(std::string strLaterality, std::string strMQCMCode, vtkMatrix4x4* pPatientMatrix)
{
	for (const auto& viewCode : { "LM", }) {
		if (strMQCMCode == viewCode) {
			if (pPatientMatrix && (pPatientMatrix->GetElement(1, 0) > 0)) {
				// Flip the ZAxis
				m_aryAxesCosines[8] = -1;
			}
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

smMammographyViewOrientatationHelper::smMammographyViewOrientatationHelper(std::shared_ptr<smDicomReader> pReader) : m_pPrivate{std::make_unique<Private>(pReader)}
{

}

/////////////////////////////////////////////////////////////////////////////////////////

smMammographyViewOrientatationHelper::~smMammographyViewOrientatationHelper()
{

}

/////////////////////////////////////////////////////////////////////////////////////////

bool smMammographyViewOrientatationHelper::Update()
{
	bool retVal = (m_pPrivate->m_pReader != nullptr);

	if (retVal) {

		auto strViewCodeSequence = m_pPrivate->m_pReader->GetViewCodeSequence();
		std::string strLaterality = m_pPrivate->m_pReader->GetImageLaterality();
		std::string strPatientOrientation = m_pPrivate->m_pReader->GetPatientOrientation();

		retVal = !strViewCodeSequence.empty();
		if (retVal) {
			std::string strMQCMCode = m_pPrivate->m_pReader->GetMammographyACR_MQCM_CodeFromViewCodeSequence(strViewCodeSequence);
			retVal = !strMQCMCode.empty();
			if (retVal) {

#ifdef DEBUG_ORIENTATION
				std::ostringstream stream;
				stream << "Patient Matrix:" << std::endl;
				auto pPatientMatrix = m_pPrivate->m_pReader->GetPatientMatrix();
				pPatientMatrix->Print(stream);

				stream << "View Code: " << strMQCMCode << std::endl;
				stream << "Laterality: " << strLaterality << std::endl;
				stream << "PatientOrientation: " << strPatientOrientation << std::endl;

				QLOG_DEBUG() << QString::fromStdString(stream.str());
#endif 

				m_pPrivate->m_nViewConvention = m_pPrivate->getProperViewConventionForImage(strLaterality, strPatientOrientation, strMQCMCode, pPatientMatrix);
				m_pPrivate->updateAxesDirectionCosines(strLaterality, strMQCMCode, pPatientMatrix);
			}
		}
		else {
			QLOG_WARN_ASSERT() << QString("In %1 the view code is empty").arg(__FUNCTION__);
		}
	}

	m_pPrivate->m_bInitialized = retVal;

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smMammographyViewOrientatationHelper::hasValidData() const
{
	return m_pPrivate->m_bInitialized;
}

/////////////////////////////////////////////////////////////////////////////////////////

const std::array<double, 9>& smMammographyViewOrientatationHelper::getAxesDirectionCosines() const
{
	return m_pPrivate->m_aryAxesCosines;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smMammographyViewOrientatationHelper::hasIdentityAxesDirectionCosines() const
{
	return m_pPrivate->m_aryAxesCosines == IdentityCosines;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smMammographyViewOrientatationHelper::isIdentityCosines(const AxesDirectionCosines& directionCosines)
{
	return IdentityCosines == directionCosines;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smMammographyViewOrientatationHelper::isAxesDirectionCosinesFilpZ() const
{
	return m_pPrivate->m_aryAxesCosines == FlipZCosines;
}

/////////////////////////////////////////////////////////////////////////////////////////

int smMammographyViewOrientatationHelper::getProperViewConventionForImage() const
{
	return m_pPrivate->m_nViewConvention;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smMammographyViewOrientatationHelper::setAxesDirectionCosines(const AxesDirectionCosines& cosines)
{
	m_pPrivate->m_aryAxesCosines = cosines;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smMammographyViewOrientatationHelper::isFFDM() const
{
	return m_pPrivate->isFFDM();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smMammographyViewOrientatationHelper::isDBT_BTO() const
{
	return m_pPrivate->isDBT_BTO();
}

/////////////////////////////////////////////////////////////////////////////////////////
