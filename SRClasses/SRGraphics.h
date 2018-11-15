#ifndef __SRCHANNELGRAPHICS__
#define __SRCHANNELGRAPHICS__

#include "IControl.h"
#include <vector>
#include <algorithm>

namespace SRClasses {

class Point {
public:
	unsigned int uid;
	double x;
	double y;
	bool operator < (const Point& point) const { return (this->x < point.x); };
	bool operator > (const Point& point) const { return (this->x > point.x); };
	bool operator == (const Point& point) const { return (this->uid == point.uid); };
};

class SRChannelGraphics : public IControl {
protected:
	std::vector<Point> points;
	Point selected;
	bool isDragging;
	unsigned int counter;

	double convertToGraphicX(double value) {
		double min = (double)this->mRECT.L;
		double distance = (double)this->mRECT.W();
		return value * distance + min;
	};
	double convertToPercentX(double value) {
		double min = (double)this->mRECT.L;
		double position = value - min;
		double distance = (double)this->mRECT.W();
		return position / distance;
	};
	double convertToGraphicY(double value) {
		double min = (double)this->mRECT.T;
		double distance = (double)this->mRECT.H();
		// We use (1 - value) as the max value 1 is located on top of graphics and not bottom
		return (1 - value) * distance + min;
	};
	double convertToPercentY(double value) {
		double min = (double)this->mRECT.T;
		double position = value - min;
		double distance = (double)this->mRECT.H();
		// We return the 1 - distance as the value 1 is located on top of graphics and not bottom
		return 1 - position / distance;
	};

public:
	SRChannelGraphics(IPlugBase *pPlug, IRECT pR) : IControl(pPlug, pR) {};
	~SRChannelGraphics() {};

	bool IsDirty() { return true; };

	bool Draw(IGraphics *pGraphics) {
		IColor color(255, 50, 200, 20);
		Point previous;
		// Little trick, no "real" points got uid = 0, so we know it's
		// not a real point, and we should avoid doing line with it...
		previous.uid = 0;

		for (std::vector<Point>::iterator it = points.begin(); it != points.end(); ++it) {
			Point current = *it;
			// We draw the point
			pGraphics->DrawCircle(&color, convertToGraphicX(current.x), convertToGraphicY(current.y), 3, 0, true);

			// The previous point is a valid point, we draw line also
			if (previous.uid > 0) {
				pGraphics->DrawLine(&color,
					convertToGraphicX(previous.x), convertToGraphicY(previous.y),
					convertToGraphicX(current.x), convertToGraphicY(current.y),
					0, true);
			}

			// We update the previous point
			previous = current;
		}

		/* TODO:
		I didn't put it here, to make you think
		But you need few more cases to handle:
		- when there is no points on the graphic
		- when there is a single point on the graphic
		- what to draw BEFORE the first point
		- what to draw AFTER the last point
		*/
		return true;
	};

	Point getPoint(double x, double y, double epsilon) {
		for (std::vector<Point>::iterator it = points.begin(); it != points.end(); ++it) {
			Point point = *it;
			double xGraphic = convertToGraphicX(point.x);
			double yGraphic = convertToGraphicY(point.y);

			if (
				// X check
				(xGraphic - epsilon < x && xGraphic + epsilon > x) &&
				// Y check
				(yGraphic - epsilon < y && yGraphic + epsilon > y)
				) {
				return point;
			}
		}

		// Nothing found, we return a "blank" point
		Point none;
		none.uid = 0;
		return none;
	};

	void OnMouseDblClick(int x, int y, IMouseMod* pMouseMod) {
		Point imHere = getPoint(x, y, 6);

		// As we said, the uid = 0 means no point
		if (imHere.uid == 0) {
			Point newPoint;
			newPoint.x = convertToPercentX(x);
			newPoint.y = convertToPercentY(y);
			newPoint.uid = counter++;
			points.push_back(newPoint);
			// And we sort it!
			std::sort(points.begin(), points.end());
			SetDirty();
		}
		else {
			// We delete the point
			points.erase(std::remove(points.begin(), points.end(), imHere), points.end());
			SetDirty();
		}
	};

	void OnMouseUp(int x, int y, IMouseMod* pMouseMod) {
		isDragging = false;
	};

	void OnMouseDown(int x, int y, IMouseMod* pMouseMod) {
		Point imHere = getPoint(x, y, 6);

		if (imHere.uid == 0) {
			// We erase selected
			Point none;
			none.uid = 0;
			selected = none;
			// Not needed, but who knows.
			isDragging = false;
			SetDirty();
		}
		else {
			selected = imHere;
			isDragging = true;
			SetDirty();
		}
	};

	void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMouseMod) {
		if (selected.uid == 0 || isDragging == false) {
			// Nothing to do
			return;
		}

		// We search for our points
		// As selected is not a pointer
		// If we modify it, it will be meaningless...
		std::vector<Point>::iterator it = std::find(points.begin(), points.end(), selected);

		if (it != points.end()) {
			(&(*it))->x = convertToPercentX(x);
			(&(*it))->y = convertToPercentY(y);
		}

		// And we ask to render
		SetDirty();
	};
};
#endif

}