add_subdirectory(pics)

set(kbackup_SRCS
    Archiver.cxx
    MainWindow.cxx
    Selector.cxx
    main.cxx
    MainWidget.cxx
    SettingsDialog.cxx
    )

ki18n_wrap_ui(kbackup_SRCS MainWidgetBase.ui SettingsDialog.ui)

add_executable(kbackup ${kbackup_SRCS})
target_link_libraries(kbackup
                      Qt5::Core
                      Qt5::Widgets
                      KF5::I18n
                      KF5::KIOCore
                      KF5::KIOFileWidgets
                      KF5::KIOWidgets
                      KF5::WidgetsAddons
                      KF5::XmlGui
                      KF5::Notifications
                      KF5::IconThemes
                      KF5::Archive
)

install(TARGETS kbackup ${INSTALL_TARGETS_DEFAULT_ARGS})

find_package(SharedMimeInfo REQUIRED)

install(FILES kbackup.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})

install(FILES  org.kde.kbackup.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES  kbackupui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kbackup)
install(FILES org.kde.kbackup.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ecm_install_icons(ICONS
                  16-apps-kbackup.png
                  32-apps-kbackup.png
                  16-mimetypes-text-x-kbp.png
                  32-mimetypes-text-x-kbp.png
                  DESTINATION ${ICON_INSTALL_DIR}
                  THEME hicolor)
