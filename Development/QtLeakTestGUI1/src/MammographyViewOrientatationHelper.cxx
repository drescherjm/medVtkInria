#include "MammographyViewOrientatationHelper.h"
#include "DicomReader.h"
#include "smvtkImageView2D.h"

#include <array>
#include <vtkMatrix4x4.h>

/////////////////////////////////////////////////////////////////////////////////////////

using VTKView = smvtkImageView2D;

static constexpr std::array<double, 9> IdentityCosines{ 1,0,0,0,1,0,0,0,1 };
static constexpr std::array<double, 9> FlipZCosines{ 1,0,0,0,1,0,0,0,-1 };

/////////////////////////////////////////////////////////////////////////////////////////

class MammographyViewOrientatationHelper::Private
{
public:
	Private(std::shared_ptr<DicomReader> pReader);
public:
	int getProperViewConventionForImage(std::string strLaterality, std::string strMQCMCode, vtkMatrix4x4* pPatientMatrix);
	static int GetCodeForDBT_LMLO_LCC(vtkMatrix4x4* pPatientMatrix);
	static int GetCodeForDBT_RMLO_RCC(vtkMatrix4x4* pPatientMatrix);
	static int GetCodeForDBT_LLM(vtkMatrix4x4* pPatientMatrix);
	static int GetCodeForDBT_RLM(vtkMatrix4x4* pPatientMatrix);
	static int GetCodeForDBT_LXCCL(vtkMatrix4x4* pPatientMatrix);
	static int GetCodeForDBT_RXCCL(vtkMatrix4x4* pPatientMatrix);
	void updateAxesDirectionCosines(std::string strLaterality, std::string strMQCMCode, vtkMatrix4x4* pPatientMatrix);
	bool isFFDM() const;
	bool isDBT_BTO() const;
public:
	std::shared_ptr<DicomReader>	m_pReader;
	bool							m_bInitialized{ false };
	int								m_nViewConvention{ VTKView::VIEW_CONVENTION_LUNG_HFS_AXIAL_VIEW_AXIAL };
	std::array<double, 9>			m_aryAxesCosines{ IdentityCosines };
};

/////////////////////////////////////////////////////////////////////////////////////////

MammographyViewOrientatationHelper::Private::Private(std::shared_ptr<DicomReader> pReader) : m_pReader{ pReader }
{

}

/////////////////////////////////////////////////////////////////////////////////////////

bool MammographyViewOrientatationHelper::Private::isFFDM() const
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

bool MammographyViewOrientatationHelper::Private::isDBT_BTO() const
{
	bool retVal{ false };

	auto dicomSOPClass = m_pReader->GetSOPClassUID();

	if (!dicomSOPClass.empty()) {
		retVal = (dicomSOPClass == "1.2.840.10008.5.1.4.1.1.13.1.3");
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

int MammographyViewOrientatationHelper::Private::GetCodeForDBT_LMLO_LCC(vtkMatrix4x4* pPatientMatrix)
{
	if (pPatientMatrix) {
		if (pPatientMatrix->GetElement(1, 0) < 0) {
			return 3;
		}
	}
	return 4;
}

/////////////////////////////////////////////////////////////////////////////////////////

int MammographyViewOrientatationHelper::Private::GetCodeForDBT_RMLO_RCC(vtkMatrix4x4* pPatientMatrix)
{
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////

int MammographyViewOrientatationHelper::Private::GetCodeForDBT_LLM(vtkMatrix4x4* pPatientMatrix)
{
	if (pPatientMatrix) {
		if (pPatientMatrix->GetElement(1, 0) > 0) {
			return 8;
		}
	}
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////

int MammographyViewOrientatationHelper::Private::GetCodeForDBT_RLM(vtkMatrix4x4* pPatientMatrix)
{
	return 3;
}

/////////////////////////////////////////////////////////////////////////////////////////

int MammographyViewOrientatationHelper::Private::GetCodeForDBT_LXCCL(vtkMatrix4x4* pPatientMatrix)
{
	return 4;
}

/////////////////////////////////////////////////////////////////////////////////////////

int MammographyViewOrientatationHelper::Private::GetCodeForDBT_RXCCL(vtkMatrix4x4* pPatientMatrix)
{
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////

int MammographyViewOrientatationHelper::Private::getProperViewConventionForImage(std::string strLaterality, std::string strMQCMCode, vtkMatrix4x4* pPatientMatrix)
{
	int retVal = VTKView::VIEW_CONVENTION_LUNG_HFS_AXIAL_VIEW_AXIAL;

	if (!strLaterality.empty() && !strMQCMCode.empty()) {

		std::string strCode = strLaterality[0] + strMQCMCode;

		bool bMultiframe = m_pReader->isMultiframeDicom();
		if (bMultiframe) {

			using DetectionFunction = std::function<int(vtkMatrix4x4*)>;

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
				retVal = it->second(pPatientMatrix);
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
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void MammographyViewOrientatationHelper::Private::updateAxesDirectionCosines(std::string strLaterality, std::string strMQCMCode, vtkMatrix4x4* pPatientMatrix)
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

MammographyViewOrientatationHelper::MammographyViewOrientatationHelper(std::shared_ptr<DicomReader> pReader) : m_pPrivate{std::make_unique<Private>(pReader)}
{

}

/////////////////////////////////////////////////////////////////////////////////////////

MammographyViewOrientatationHelper::~MammographyViewOrientatationHelper()
{

}

/////////////////////////////////////////////////////////////////////////////////////////

bool MammographyViewOrientatationHelper::Update()
{
	bool retVal = (m_pPrivate->m_pReader != nullptr);

	if (retVal) {

		std::cout << "Patient Matrix:" << std::endl;
		auto pPatientMatrix = m_pPrivate->m_pReader->GetPatientMatrix();
		pPatientMatrix->Print(std::cout);

		auto strViewCodeSequence = m_pPrivate->m_pReader->GetViewCodeSequence();
		std::string strLaterality = m_pPrivate->m_pReader->GetImageLaterality();

		retVal = !strViewCodeSequence.empty();
		if (retVal) {
			std::string strMQCMCode = m_pPrivate->m_pReader->GetMammographyACR_MQCM_CodeFromViewCodeSequence(strViewCodeSequence);
			retVal = !strMQCMCode.empty();
			if (retVal) {
				m_pPrivate->m_nViewConvention = m_pPrivate->getProperViewConventionForImage(strLaterality, strMQCMCode, pPatientMatrix);
				m_pPrivate->updateAxesDirectionCosines(strLaterality, strMQCMCode, pPatientMatrix);
			}
		}
	}

	m_pPrivate->m_bInitialized = retVal;

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool MammographyViewOrientatationHelper::hasValidData() const
{
	return m_pPrivate->m_bInitialized;
}

/////////////////////////////////////////////////////////////////////////////////////////

const std::array<double, 9>& MammographyViewOrientatationHelper::getAxesDirectionCosines() const
{
	return m_pPrivate->m_aryAxesCosines;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool MammographyViewOrientatationHelper::hasIdentityAxesDirectionCosines() const
{
	return m_pPrivate->m_aryAxesCosines == IdentityCosines;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool MammographyViewOrientatationHelper::isAxesDirectionCosinesFilpZ() const
{
	return m_pPrivate->m_aryAxesCosines == FlipZCosines;
}

/////////////////////////////////////////////////////////////////////////////////////////

int MammographyViewOrientatationHelper::getProperViewConventionForImage() const
{
	return m_pPrivate->m_nViewConvention;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool MammographyViewOrientatationHelper::isFFDM() const
{
	return m_pPrivate->isFFDM();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool MammographyViewOrientatationHelper::isDBT_BTO() const
{
	return m_pPrivate->isDBT_BTO();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool MammographyViewOrientatationHelper::isIdentityCosines(const AxesDirectionCosines& directionCosines)
{
	return IdentityCosines == directionCosines;
}

/////////////////////////////////////////////////////////////////////////////////////////
