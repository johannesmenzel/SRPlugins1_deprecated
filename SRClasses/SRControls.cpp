#include "SRControls.h"

namespace SRPlugins {
	namespace SRControls {
		bool IKnobMultiControlText::Draw(IGraphics * pGraphics) {
			int i = 1 + int(0.5 + mValue * (double)(mBitmap.N - 1));
			i = BOUNDED(i, 1, mBitmap.N);
			pGraphics->DrawBitmap(&mBitmap, &mImgRECT, i, &mBlend);

			char disp[20];
			mPlug->GetParam(mParamIdx)->GetDisplayForHost(disp);
			std::string final = disp;
			if (!mEnd.empty()) {
				final += " " + mEnd;
			}

			if (CSTR_NOT_EMPTY(disp)) {
				return pGraphics->DrawIText(&mText, const_cast<char*>(final.c_str()), &mTextRECT);
			}
			return true;
		}
		void IKnobMultiControlText::OnMouseDown(int x, int y, IMouseMod * pMod) {
			if (mTextRECT.Contains(x, y)) PromptUserInput(&mTextRECT);
#ifdef RTAS_API
			else if (pMod->A) {
				if (mDefaultValue >= 0.0) {
					mValue = mDefaultValue;
					SetDirty();
				}
			}
#endif
			else {
				OnMouseDrag(x, y, 0, 0, pMod);
			}
		}
		void IKnobMultiControlText::OnMouseDblClick(int x, int y, IMouseMod * pMod) {
#ifdef RTAS_API
			PromptUserInput(&mTextRECT);
#else
			if (mDefaultValue >= 0.0) {
				mValue = mDefaultValue;
				SetDirty();
			}
#endif
		}	
		bool SRMultiMeter::Draw(IGraphics *) {
			return true;
		}
		bool IPeakMeterVert::Draw(IGraphics * pGraphics) {
			pGraphics->FillIRect(&meterbg, &mRECT);
			IRECT filledBit = IRECT(mRECT.L, mRECT.T + int((1 - mValue) * mRECT.H()), mRECT.R, mRECT.B);
			pGraphics->FillIRect(&meterfg, &filledBit);
			return true;
		}
		bool IGrMeterVert::Draw(IGraphics * pGraphics)
		{
			pGraphics->FillIRect(&meterbg, &mRECT);
			IRECT filledBit = IRECT(mRECT.L, mRECT.T, mRECT.R, mRECT.B - int(mValue * mRECT.H()));
			pGraphics->FillIRect(&meterfg, &filledBit);
			return true;
		}
		bool IPeakMeterHoriz::Draw(IGraphics * pGraphics) {
			pGraphics->FillIRect(&meterbg, &mRECT);
			IRECT filledBit = IRECT(mRECT.L, mRECT.T, mRECT.L + int(mValue * mRECT.W()), mRECT.B);
			pGraphics->FillIRect(&meterfg, &filledBit);
			return true;
		}
		bool ITestPopupMenu::Draw(IGraphics * pGraphics) {
			return pGraphics->FillIRect(&COLOR_WHITE, &mRECT);;
		}
		void ITestPopupMenu::OnMouseDown(int x, int y, IMouseMod * pMod) {
			doPopupMenu();

			Redraw(); // seems to need this
			SetDirty();
		}
		void ITestPopupMenu::doPopupMenu() {
			IPopupMenu* selectedMenu = mPlug->GetGUI()->CreateIPopupMenu(&mMainMenu, &mRECT);

			if (selectedMenu == &mMainMenu) {
				int itemChosen = selectedMenu->GetChosenItemIdx();
				selectedMenu->CheckItemAlone(itemChosen);
				DBGMSG("item chosen, main menu %i\n", itemChosen);
			}
			else if (selectedMenu == &mSubMenu) {
				int itemChosen = selectedMenu->GetChosenItemIdx();
				selectedMenu->CheckItemAlone(itemChosen);
				DBGMSG("item chosen, sub menu %i\n", itemChosen);
			}
			else {
				DBGMSG("nothing chosen\n");
			}
		}
		bool ITestPopupMenuB::Draw(IGraphics * pGraphics) {
			return pGraphics->FillIRect(&COLOR_WHITE, &mRECT);;
		}
		void ITestPopupMenuB::OnMouseDown(int x, int y, IMouseMod * pMod) {
			doPopupMenu();

			Redraw(); // seems to need this
			SetDirty();
		}
		void ITestPopupMenuB::doPopupMenu() {
			IPopupMenu* selectedMenu = mPlug->GetGUI()->CreateIPopupMenu(&mMainMenu, &mRECT);

			if (selectedMenu) {
				int idx = selectedMenu->GetChosenItemIdx();
				selectedMenu->CheckItem(idx, !selectedMenu->IsItemChecked(idx));

				WDL_String checkedItems;

				checkedItems.Append("checked: ", 1024);

				for (int i = 0; i < selectedMenu->GetNItems(); i++) {
					checkedItems.AppendFormatted(1024, "%i ", selectedMenu->IsItemChecked(i));
				}

				DBGMSG("%s\n", checkedItems.Get());
			}
		}
		bool IPopUpMenuControl::Draw(IGraphics * pGraphics) {
			pGraphics->FillIRect(&mColorBG, &mRECT);

			char disp[32];
			mPlug->GetParam(mParamIdx)->GetDisplayForHost(disp);

			if (CSTR_NOT_EMPTY(disp)) {
				return pGraphics->DrawIText(&mText, disp, &mRECT);
			}

			return true;
		}
		void IPopUpMenuControl::OnMouseDown(int x, int y, IMouseMod * pMod) {
			if (pMod->L) {
				PromptUserInput(&mRECT);
			}

			mPlug->GetGUI()->SetAllControlsDirty();
		}
		bool IPresetMenu::Draw(IGraphics * pGraphics) {
			int pNumber = mPlug->GetCurrentPresetIdx();
			mDisp.SetFormatted(32, "%02d: %s", pNumber + 1, mPlug->GetPresetName(pNumber));

			IColor colorBg = IColor(50, 0, 0, 0);
			pGraphics->FillIRect(&colorBg, &mRECT);

			if (CSTR_NOT_EMPTY(mDisp.Get()))
			{
				return pGraphics->DrawIText(&mText, mDisp.Get(), &mRECT);
			}

			return true;
		}
		void IPresetMenu::OnMouseDown(int x, int y, IMouseMod * pMod) {
			if (pMod->R) {
				const char* pname = mPlug->GetPresetName(mPlug->GetCurrentPresetIdx());
				mPlug->GetGUI()->CreateTextEntry(this, &mText, &mRECT, pname);
			}
			else {
				doPopupMenu();
			}

			Redraw(); // seems to need this
			SetDirty();
		}
		void IPresetMenu::doPopupMenu() {
			int numItems = mPlug->NPresets();
			IPopupMenu menu;

			IGraphics* gui = mPlug->GetGUI();

			int currentPresetIdx = mPlug->GetCurrentPresetIdx();

			for (int i = 0; i < numItems; i++) {
				const char* str = mPlug->GetPresetName(i);
				if (i == currentPresetIdx)
					menu.AddItem(str, -1, IPopupMenuItem::kChecked);
				else
					menu.AddItem(str);
			}

			menu.SetPrefix(2);

			if (gui->CreateIPopupMenu(&menu, &mRECT)) {
				int itemChosen = menu.GetChosenItemIdx();

				if (itemChosen > -1) {
					mPlug->RestorePreset(itemChosen);
					mPlug->InformHostOfProgramChange();
					mPlug->DirtyParameters();
				}
			}
		}
		void IPresetMenu::TextFromTextEntry(const char * txt) {
			WDL_String safeName;
			safeName.Set(txt, MAX_PRESET_NAME_LEN);

			mPlug->ModifyCurrentPreset(safeName.Get());
			mPlug->InformHostOfProgramChange();
			mPlug->DirtyParameters();
			SetDirty(false);
		}
		bool IKeyCatcher::OnKeyDown(int x, int y, int key) {
			switch (key) {
				//case KEY_SPACE:
				///  DBGMSG("Space\n");
				//  return true;
			case KEY_LEFTARROW:;
				DBGMSG("Left\n");
				return true;
			case KEY_RIGHTARROW:
				DBGMSG("Right\n");
				return true;
			case KEY_UPARROW:;
				DBGMSG("Up\n");
				return true;
			case KEY_DOWNARROW:
				DBGMSG("Down\n");
				return true;
			default:
				return false;
			}
		}
		bool ITempoDisplay::Draw(IGraphics * pGraphics) {
			mDisplay.SetFormatted(80, "Tempo: %f, SamplePos: %i, PPQPos: %f", mTimeInfo->mTempo, (int)mTimeInfo->mSamplePos, mTimeInfo->mPPQPos);
			return pGraphics->DrawIText(&mText, mDisplay.Get(), &mRECT);
		}
	}
} // end namespace SRControls