#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "string_finder/worker.h"
#include <QMainWindow>
#include <QThread>
#include <QTime>
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void make_connections();
    void make_ui();

    void show_progress_bar();

private slots:
    void open_directory();
    void set_stage(QString const&);
    void search_init();
    void stop_clicked();

    void show_about_qt();
    void show_help();

public slots:
    void indexing_started();
    void indexing_finished();

    void searching_started();
    void searching_finished();

    void set_steps_count(size_t);
    void set_current_step(size_t);

    void add_result(QString const& file, QString const& slice);

signals:
    void set_directory_to_search(QString const&);
    void start_search();
    void stop_search();
    void stop_indexing();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    QThread* index_thread, *search_thread;
    IndexWorker* index_worker;
    std::vector<std::unique_ptr<SearchEngine> > searchers;
    QTime timer;
    QString stage;
};

#endif // MAINWINDOW_H
