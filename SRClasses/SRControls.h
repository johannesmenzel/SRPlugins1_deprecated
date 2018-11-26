#pragma once
#include "IPlug_include_in_plug_hdr.h"
#include "SRDynamics.h"
#include <string>

namespace SRPlugins {

	namespace SRControls {



		//------------------------------------
		// Generic Template
		// -----------------------------------

		//class SRGenericControl : public IControl {
		//public:
		//	SRGenericControl(IPlugBase *pPlug, IRECT pR) : IControl(pPlug, pR) {}
		//	~SRGenericControl() {}
		//	bool IsDirty() { return true; }

		//	bool Draw(IGraphics*) {

		//	};
		//};

		//------------------------------------
		// Parameter Controls (Knob)
		// -----------------------------------

		class IKnobMultiControlText : public IKnobControl {
		public:
			IKnobMultiControlText(IPlugBase* pPlug, IRECT pR, int paramIdx, IBitmap* pBitmap, IText* pText, const std::string& end, EDirection direction = kVertical,
				double gearing = DEFAULT_GEARING)
				: IKnobControl(pPlug, pR, paramIdx, direction, gearing), mBitmap(*pBitmap), mEnd(end) {
				mText = *pText;
				mTextRECT = IRECT(mRECT.L, mRECT.B - mText.mSize, mRECT.R, mRECT.B);
				mImgRECT = IRECT(mRECT.L, mRECT.T, &mBitmap);
				mDisablePrompt = false;
				mMOWhenGreyed = true;
			}
			~IKnobMultiControlText() {}
			bool IKnobMultiControlText::GetMOWhenGrayed() { return true; }
			bool Draw(IGraphics* pGraphics);
			void OnMouseDown(int x, int y, IMouseMod* pMod);
			void OnMouseDblClick(int x, int y, IMouseMod* pMod);
		private:
			IRECT mTextRECT, mImgRECT;
			IBitmap mBitmap;
			std::string mEnd;
		};



		//------------------------------------
		// Meters
		// -----------------------------------

		class SRMultiMeter : public IControl {
		public:
			SRMultiMeter(IPlugBase *pPlug, IRECT pR) : IControl(pPlug, pR) {}
			~SRMultiMeter() {}
			bool IsDirty() { return true; }
			bool Draw(IGraphics*);
		};


		class IPeakMeterVert : public IControl {
		public:
			IPeakMeterVert(IPlugBase* pPlug, IRECT pR, IColor background, IColor foreground)
				: IControl(pPlug, pR) {
				meterbg = background;
				meterfg = foreground;
			}
			~IPeakMeterVert() {}
			bool IsDirty() { return true; }
			bool Draw(IGraphics* pGraphics);
		protected:
			IColor meterfg, meterbg;
		};


		class IGrMeterVert : public IPeakMeterVert {
		public:
			IGrMeterVert(IPlugBase* pPlug, IRECT pR, IColor background, IColor foreground)
				: IPeakMeterVert(pPlug, pR, background, foreground) {
				mValue = 1.;
			}
			~IGrMeterVert() {}
			bool IsDirty() { return true; }
			bool Draw(IGraphics* pGraphics);
		};


		class IPeakMeterHoriz : public IPeakMeterVert {
		public:
			IPeakMeterHoriz(IPlugBase* pPlug, IRECT pR, IColor background, IColor foreground)
				: IPeakMeterVert(pPlug, pR, background, foreground) {}
			~IPeakMeterHoriz() {}
			bool IsDirty() { return true; }
			bool Draw(IGraphics* pGraphics);
		};


		class ITempoDisplay : public IControl {
		public:
			ITempoDisplay(IPlugBase* pPlug, IRECT pR, IText* pText, ITimeInfo* pTimeInfo)
				: IControl(pPlug, pR) {
				mText = *pText;
				mTimeInfo = pTimeInfo;
			}
			bool Draw(IGraphics* pGraphics);
			bool IsDirty() { return true; }
		private:
			ITimeInfo * mTimeInfo;
			WDL_String mDisplay;
		};

		class IVariableControl : public IControl {
		public:
			IVariableControl(IPlugBase* pPlug, IRECT pR, IText* pText, double doubleValue)
				: IControl(pPlug, pR) {
				mText = *pText;
				mDoubleValue = doubleValue;
			}
			bool IsDirty() { return true; }
			bool Draw(IGraphics* pGraphics);
		private:
			double mDoubleValue;
			WDL_String mDisplay;
		};




		//------------------------------------
		// Menus
		// -----------------------------------

		// Popup Menu
		// -----------------------------------

		class ITestPopupMenu : public IControl {
		public:
			ITestPopupMenu(IPlugBase *pPlug, IRECT pR)
				: IControl(pPlug, pR, -1) {
				mMainMenu.AddItem("first item");
				mMainMenu.AddItem("second item");
				mMainMenu.AddItem("third item");

				mSubMenu.AddItem("first item");
				mSubMenu.AddItem("second item");
				mSubMenu.AddItem("third item");

				mMainMenu.AddItem("sub menu", &mSubMenu);
			}
			bool Draw(IGraphics* pGraphics);
			void OnMouseDown(int x, int y, IMouseMod* pMod);
			void doPopupMenu();
		private:
			IPopupMenu mMainMenu, mSubMenu;
		};


		class ITestPopupMenuB : public IControl {
		public:
			ITestPopupMenuB(IPlugBase *pPlug, IRECT pR)
				: IControl(pPlug, pR, -1) {
				mMainMenu.SetMultiCheck(true);
				mMainMenu.AddItem("first item");
				mMainMenu.AddItem("second item");
				mMainMenu.AddItem("third item");
			}
			bool Draw(IGraphics* pGraphics);
			void OnMouseDown(int x, int y, IMouseMod* pMod);
			void doPopupMenu();
		private:
			IPopupMenu mMainMenu;
		};


		class IPopUpMenuControl : public IControl {
		public:
			IPopUpMenuControl(IPlugBase *pPlug, IRECT pR, int paramIdx, IText* pText, IColor pColorBG)
				: IControl(pPlug, pR, paramIdx) {
				mDisablePrompt = false;
				mDblAsSingleClick = true;
				mText = *pText;
				mColorBG = pColorBG;
			}
			bool Draw(IGraphics* pGraphics);
			void OnMouseDown(int x, int y, IMouseMod* pMod);
			//void OnMouseWheel(int x, int y, IMouseMod* pMod, int d){} //TODO: popup menus seem to hog the mousewheel
		private:
			IColor mColorBG;
		};



		// Preset Menu
		// -----------------------------------

		class IPresetMenu : public IControl {
		public:
			IPresetMenu(IPlugBase *pPlug, IRECT pR, IText* pText)
				: IControl(pPlug, pR, -1) {
				mTextEntryLength = MAX_PRESET_NAME_LEN - 3;
				mText = *pText;
			}
			bool Draw(IGraphics* pGraphics);
			void OnMouseDown(int x, int y, IMouseMod* pMod);
			void doPopupMenu();
			void TextFromTextEntry(const char* txt);
		private:
			WDL_String mDisp;
		};



		//------------------------------------
		// Utilities
		// -----------------------------------

		// Key catcher is an icontrol but only its OnKeyDown() is called... after all the other controls have been tested to see if they want keyboard input

		class IKeyCatcher : public IControl {
		public:
			IKeyCatcher(IPlugBase* pPlug, IRECT pR)
				: IControl(pPlug, pR) {}
			// this never gets called but is needed for an IControl
			bool Draw(IGraphics* pGraphics) { return false; }
			bool OnKeyDown(int x, int y, int key);
		};


		class Point {
		public:
			unsigned int uid;
			double x;
			double y;
			bool operator < (const Point& point) const { return (this->x < point.x); };
			bool operator > (const Point& point) const { return (this->x > point.x); };
			bool operator == (const Point& point) const { return (this->uid == point.uid); };
		};

	} // End namespace SRControls
} // End namespace SRPlugins