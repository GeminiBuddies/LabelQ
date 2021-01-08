#include <model/Label.h>

Label::Label(const QPoint &position, const QString &translation) : position(position), translation(translation) {}

Label::Label(Label &&label) noexcept : position(label.position), translation(std::move(label.translation)) {}
