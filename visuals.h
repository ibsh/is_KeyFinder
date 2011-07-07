#ifndef VISUALS_H
#define VISUALS_H

#include <QString>
#include <QImage>
#include <QColor>

#include <vector>

// Singleton. Holds the string names of the keys, and the colours associated with various things

class Visuals{
public:
	static Visuals* getInstance();
	std::vector<QString> keyNames;
	std::vector<QColor> keyColours;
	void setChromagramColours(QImage&, int) const;
private:
	Visuals();
	static Visuals* inst;
};

#endif // VISUALS_H
