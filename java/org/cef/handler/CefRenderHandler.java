// Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

package org.cef.handler;

import org.cef.browser.CefBrowser;
import org.cef.callback.CefDragData;
import org.cef.misc.CefRange;

import java.awt.Point;
import java.awt.Rectangle;
import java.nio.ByteBuffer;

/**
 * Implement this interface to handle events when window rendering is disabled.
 * The methods of this class will be called on the UI thread.
 */
public interface CefRenderHandler {
    /**
     * Retrieve the view rectangle.
     * @param browser The browser generating the event.
     * @return The view rectangle.
     */
    public Rectangle getViewRect(CefBrowser browser);

    /**
     * Retrieve the screen info.
     * @param browser The browser generating the event.
     * @param screenInfo The screenInfo
     * @return True if this callback was handled.  False to fallback to defaults.
     */
    public boolean getScreenInfo(CefBrowser browser, CefScreenInfo screenInfo);

    /**
     * Retrieve the screen point for the specified view point.
     * @param browser The browser generating the event.
     * @param viewPoint The point in the view.
     * @return The screen point.
     */
    public Point getScreenPoint(CefBrowser browser, Point viewPoint);

    /**
     * Retrieve the scale factor of the screen the browser is shown at.
     */
    public double getDeviceScaleFactor(CefBrowser browser);

    /**
     * Show or hide the popup window.
     * @param browser The browser generating the event.
     * @param show True if the popup window is being shown.
     */
    public void onPopupShow(CefBrowser browser, boolean show);

    /**
     * Size the popup window.
     * @param browser The browser generating the event.
     * @param size Size of the popup window.
     */
    public void onPopupSize(CefBrowser browser, Rectangle size);

    /**
     * Handle painting.
     * @param browser The browser generating the event.
     * @param popup True if painting a popup window.
     * @param dirtyRects Array of dirty regions.
     * @param buffer Pixel buffer for the whole window.
     * @param width Width of the buffer.
     * @param height Height of the buffer.
     */
    public void onPaint(CefBrowser browser, boolean popup, Rectangle[] dirtyRects,
            ByteBuffer buffer, int width, int height);

    /**
     * Handle cursor changes.
     * @param browser The browser generating the event.
     * @param cursorType The new cursor type.
     * @return true if the cursor change was handled.
     */
    public boolean onCursorChange(CefBrowser browser, int cursorType);

    /**
     * Called when the user starts dragging content in the web view. Contextual
     * information about the dragged content is supplied by dragData.
     * OS APIs that run a system message loop may be used within the
     * StartDragging call.
     *
     * Return false to abort the drag operation. Don't call any of
     * CefBrowser-dragSource*Ended* methods after returning false.
     *
     * Return true to handle the drag operation. Call
     * CefBrowser.dragSourceEndedAt and CefBrowser.ragSourceSystemDragEnded either
     * synchronously or asynchronously to inform the web view that the drag
     * operation has ended.
     * @param browser The browser generating the event.
     * @param dragData Contextual information about the dragged content
     * @param mask Describes the allowed operation (none, move, copy, link).
     * @param x Coordinate within CefBrowser
     * @param y Coordinate within CefBrowser
     * @return false to abort the drag operation or true to handle the drag operation.
     */
    public boolean startDragging(CefBrowser browser, CefDragData dragData, int mask, int x, int y);

    /**
     * Called when the web view wants to update the mouse cursor during a
     * drag & drop operation.
     *
     * @param browser The browser generating the event.
     * @param operation Describes the allowed operation (none, move, copy, link).
     */
    public void updateDragCursor(CefBrowser browser, int operation);

    /**
     * Called when the IME composition range has changed.
     *
     * @param browser the browser for that the composition has been changed
     * @param selectionRange the range of characters that have been selected
     * @param characterBounds the bounds of each character in view coordinates
     */
    public default void OnImeCompositionRangeChanged(CefBrowser browser, CefRange selectionRange, Rectangle[] characterBounds) {
        // To be removed
    }

    /**
     * Called when text selection has changed
     *
     * @param browser the browser for that the selection has been changed
     * @param selectedText the currently selected text
     * @param selectionRange the character range
     */
    public default void OnTextSelectionChanged(CefBrowser browser, String selectedText, CefRange selectionRange) {
        // To be removed
    }
}
