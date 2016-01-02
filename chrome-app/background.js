chrome.app.runtime.onLaunched.addListener(function() {
  chrome.app.window.create('window.html', {
    'innerBounds': {
      'width': 1024,
      'height': 576
    }
  });
});