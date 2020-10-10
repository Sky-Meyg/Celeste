#include "panes.h"

#include <QVBoxLayout>
#include <QLabel>

StatusPane::StatusPane(QWidget *parent) : Pane(parent)
{
	output=new QTextEdit(this);
	output->setStyleSheet("background-color: rgba(0,0,0,0); color: white;");
	output->setFontFamily("Droid Sans Mono");
	output->setFontPointSize(10);
	output->document()->setDocumentMargin(16);
	output->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	output->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setLayout(new QVBoxLayout(this));
	layout()->setContentsMargins(0,0,0,0);
	layout()->addWidget(output);
}

void StatusPane::Print(const QString text)
{
	output->insertPlainText(text);
	output->insertPlainText("\n");
	output->moveCursor(QTextCursor::End);
}

ChatPane::ChatPane(QWidget *parent) : Pane(parent), agenda(nullptr), chat(nullptr)
{
	setLayout(new QVBoxLayout(this));
	layout()->setContentsMargins(0,0,0,0);
	layout()->setSpacing(0);

	agenda=new QLabel(this);
	agenda->setStyleSheet("background-color: rgba(0,0,0,0); color: white;");
	agenda->setFont(QFont("Copperplate Gothic Bold",20,QFont::Bold));
	agenda->setMargin(16);
	agenda->setText("Initial Implementation of Celeste Twitch Bot");
	agenda->setAlignment(Qt::AlignCenter);
	agenda->setWordWrap(true);
	layout()->addWidget(agenda);

	chat=new QTextEdit(this);
	chat->setStyleSheet("background-color: rgba(0,0,0,0); color: white;");
	chat->setFontFamily("Copperplate Gothic Bold");
	chat->setFontPointSize(16);
	chat->document()->setDefaultStyleSheet("div.user { font-family: 'Copperplate Gothic Bold'; text-align: center; font-size: 16pt; } div.message { font-family: 'Copperplate Gothic Bold'; font-size: 12pt; }");
	chat->document()->setDocumentMargin(16);
	chat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	chat->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	chat->setFrameStyle(QFrame::NoFrame);
	chat->setCursorWidth(0);
	layout()->addWidget(chat);
}

void ChatPane::Print(const QString text)
{
	chat->insertHtml(text);
	chat->insertPlainText("\n");
	chat->moveCursor(QTextCursor::End);
}

EphemeralPane::EphemeralPane(QWidget *parent) : Pane(parent)
{
	setVisible(false);
	connect(this,&EphemeralPane::Finished,this,&EphemeralPane::deleteLater);
}

VideoPane::VideoPane(const QString path,QWidget *parent) : EphemeralPane(parent)
{
	viewport=new QVideoWidget(this);
	mediaPlayer=new QMediaPlayer(this);
	mediaPlayer->setVideoOutput(viewport);
	mediaPlayer->setMedia(QUrl::fromLocalFile(path));
	connect(mediaPlayer,&QMediaPlayer::stateChanged,[this](QMediaPlayer::State state) {
		if (state == QMediaPlayer::StoppedState) emit Finished();
	});

	setLayout(new QVBoxLayout(this));
	layout()->setContentsMargins(0,0,0,0);
	layout()->addWidget(viewport);
}

void VideoPane::Show()
{
	show();
	mediaPlayer->play();
}
