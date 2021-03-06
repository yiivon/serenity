/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "EraseTool.h"
#include "PaintableWidget.h"
#include <LibGUI/GAction.h>
#include <LibGUI/GMenu.h>
#include <LibGUI/GPainter.h>

EraseTool::EraseTool()
{
}

EraseTool::~EraseTool()
{
}

Rect EraseTool::build_rect(const Point& pos, const Rect& widget_rect)
{
    const int base_eraser_size = 10;
    const int eraser_size = (base_eraser_size * m_thickness);
    const int eraser_radius = eraser_size / 2;
    const auto ex = pos.x();
    const auto ey = pos.y();
    return Rect(ex - eraser_radius, ey - eraser_radius, eraser_size, eraser_size).intersected(widget_rect);
}

void EraseTool::on_mousedown(GMouseEvent& event)
{
    if (event.button() != GMouseButton::Left && event.button() != GMouseButton::Right)
        return;
    Rect r = build_rect(event.position(), m_widget->bitmap().rect());
    GPainter painter(m_widget->bitmap());
    painter.fill_rect(r, get_color());
    m_widget->update();
}

void EraseTool::on_mousemove(GMouseEvent& event)
{
    if (!m_widget->rect().contains(event.position()))
        return;

    if (event.buttons() & GMouseButton::Left || event.buttons() & GMouseButton::Right) {
        Rect r = build_rect(event.position(), m_widget->bitmap().rect());
        GPainter painter(m_widget->bitmap());
        painter.fill_rect(r, get_color());
        m_widget->update();
    }
}

void EraseTool::on_contextmenu(GContextMenuEvent& event)
{
    if (!m_context_menu) {
        m_context_menu = GMenu::construct();

        NonnullRefPtr<GAction> eraser_color_toggler = GAction::create("Use secondary color", [&](GAction& action) {
            bool toggled = !m_use_secondary_color;
            m_use_secondary_color = toggled;
            action.set_checked(toggled);
        });
        eraser_color_toggler->set_checkable(true);
        eraser_color_toggler->set_checked(m_use_secondary_color);

        m_context_menu->add_action(eraser_color_toggler);
        m_context_menu->add_separator();
        m_context_menu->add_action(GAction::create("1", [this](auto&) {
            m_thickness = 1;
        }));
        m_context_menu->add_action(GAction::create("2", [this](auto&) {
            m_thickness = 2;
        }));
        m_context_menu->add_action(GAction::create("3", [this](auto&) {
            m_thickness = 3;
        }));
        m_context_menu->add_action(GAction::create("4", [this](auto&) {
            m_thickness = 4;
        }));
    }

    m_context_menu->popup(event.screen_position());
}

Color EraseTool::get_color() const
{
    if (m_use_secondary_color)
        return m_widget->secondary_color();
    return Color(Color::White);
}
