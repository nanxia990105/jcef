package tests.junittests;

import org.cef.CefSettings;
import org.cef.browser.CefBrowser;
import org.cef.handler.CefLifeSpanHandlerAdapter;
import org.cef.misc.CefLog;

public class LoggingLifeSpanHandler extends CefLifeSpanHandlerAdapter {
    final private CefSettings.LogSeverity myLevel;

    public LoggingLifeSpanHandler(CefSettings.LogSeverity myLevel) {
        this.myLevel = myLevel;
    }
    @Override
    public void onAfterCreated(CefBrowser browser) {
        CefLog.Log(myLevel, "onAfterCreated, bid=" + browser.getIdentifier());
    }
    @Override
    public boolean doClose(CefBrowser browser) {
        CefLog.Log(myLevel, "doClose, bid=" + browser.getIdentifier());
        return false;
    }
    @Override
    public void onBeforeClose(CefBrowser browser) {
        CefLog.Log(myLevel, "onBeforeClose, bid=" + browser.getIdentifier());
    }
}
