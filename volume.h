#pragma once

#include <QMediaPlayer>
#include <cmath>
#include <algorithm>
#include "globals.h"
#include "settings.h"

namespace Volume
{
	const QString SETTINGS_CATEGORY_VOLUME="Volume";

	class Fader : public QObject
	{
		Q_OBJECT
	public:
		Fader(QMediaPlayer *player,const QString &arguments,QObject *parent=nullptr) : Fader(player,0,static_cast<std::chrono::seconds>(0),parent)
		{
			Parse(arguments);
		}
		Fader(QMediaPlayer *player,unsigned int volume,std::chrono::seconds duration,QObject *parent=nullptr) : QObject(parent),
			settingDefaultDuration(SETTINGS_CATEGORY_VOLUME,"DefaultFadeSeconds",5),
			player(player),
			initialVolume(static_cast<unsigned int>(player->volume())),
			targetVolume(std::clamp<int>(volume,0,100)),
			startTime(TimeConvert::Now()),
			duration(TimeConvert::Milliseconds(duration))
		{
			connect(this,&Fader::AdjustmentNeeded,this,&Fader::Adjust,Qt::QueuedConnection);
		}
		void Parse(const QString &text)
		{
			QStringList values=text.split(" ",StringConvert::Split::Behavior(StringConvert::Split::Behaviors::SKIP_EMPTY_PARTS));
			if (values.size() < 1) throw std::range_error("No volume specified");
			targetVolume=StringConvert::PositiveInteger(values[0]);
			duration=values.size() > 1 ? static_cast<std::chrono::seconds>(StringConvert::PositiveInteger(values[1])) : settingDefaultDuration;
		}
		void Start()
		{
			emit Feedback(QString("Adjusting volume from %1% to %2% over %3 seconds").arg(
				StringConvert::Integer(initialVolume),
				StringConvert::Integer(targetVolume),
				StringConvert::Integer(TimeConvert::Seconds(duration).count())
			));
			emit AdjustmentNeeded();
		}
		void Stop()
		{
			disconnect(this,&Fader::AdjustmentNeeded,this,&Fader::Adjust);
			deleteLater();
		}
		void Abort()
		{
			Stop();
			player->setVolume(initialVolume); // FIXME: volume doesn't return to initial value
		}
	protected:
		Setting settingDefaultDuration;
		QMediaPlayer *player;
		unsigned int initialVolume;
		unsigned int targetVolume;
		std::chrono::milliseconds startTime;
		std::chrono::milliseconds duration;
		double Step(const double &secondsPassed)
		{
			// uses x^4 method described here: https://www.dr-lex.be/info-stuff/volumecontrols.html#ideal3
			// no need to find an ideal curve, this is a bot, not a DAW
			double totalSeconds=static_cast<double>(duration.count())/TimeConvert::Milliseconds(TimeConvert::OneSecond()).count();
			int volumeDifference=targetVolume-initialVolume;
			return std::pow(secondsPassed/totalSeconds,4)*(volumeDifference)+initialVolume;
		}
	signals:
		void AdjustmentNeeded();
		void Feedback(const QString &text);
	public slots:
		void Adjust()
		{
			double secondsPassed=static_cast<double>((TimeConvert::Now()-startTime).count())/TimeConvert::Milliseconds(TimeConvert::OneSecond()).count();
			if (secondsPassed > TimeConvert::Seconds(duration).count())
			{
				deleteLater();
				return;
			}
			double adjustment=Step(secondsPassed);
			player->setVolume(adjustment);
			emit AdjustmentNeeded();
		}
	};
}
