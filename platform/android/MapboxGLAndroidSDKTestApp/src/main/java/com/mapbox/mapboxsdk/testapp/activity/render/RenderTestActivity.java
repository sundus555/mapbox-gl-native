package com.mapbox.mapboxsdk.testapp.activity.render;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.Gravity;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.google.gson.Gson;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.snapshotter.MapSnapshotter;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import timber.log.Timber;

public class RenderTestActivity extends AppCompatActivity {

  private final Map<RenderTestDefinition, Bitmap> renderResultMap = new HashMap<>();
  private final List<MapSnapshotter> mapSnapshotterList = new ArrayList<>();

  private ImageView imageView;
  private boolean finishedRendering;

  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(imageView = new ImageView(RenderTestActivity.this));
    imageView.setLayoutParams(new FrameLayout.LayoutParams(512, 512, Gravity.CENTER));
    try {
      RenderTestDefinition[] renderTestDefinitions = createRenderTestDefinition();
      renderTests(renderTestDefinitions);
    } catch (IOException exception) {
      Timber.e(exception);
    }
  }

  private RenderTestDefinition[] createRenderTestDefinition() throws IOException {
    return new Gson().fromJson(
      new InputStreamReader(getAssets().open("render-test.json")),
      RenderTestDefinition[].class
    );
  }

  private void renderTests(RenderTestDefinition[] renderTestDefinitions) {
    for (RenderTestDefinition renderTestDefinition : renderTestDefinitions) {
      renderTest(renderTestDefinition, renderTestDefinitions.length);
    }
  }

  private void renderTest(final RenderTestDefinition renderTestDefinition, final int testSize) {
    MapSnapshotter mapSnapshotter = new MapSnapshotter(this, renderTestDefinition.toOptions());
    mapSnapshotterList.add(mapSnapshotter);
    mapSnapshotter.start(new MapboxMap.SnapshotReadyCallback() {
      @Override
      public void onSnapshotReady(Bitmap snapshot) {
        imageView.setImageBitmap(snapshot);
        renderResultMap.put(renderTestDefinition, snapshot);
        if (renderResultMap.size() == testSize) {
          writeResultsToDisk();
          finishedRendering = true;
        }
      }
    });
  }

  private void writeResultsToDisk() {
    if (isExternalStorageWritable()) {
      try {
        File testResultDir = createTestResultRootFolder();
        String basePath = testResultDir.getAbsolutePath();

        for (Map.Entry<RenderTestDefinition, Bitmap> testResult : renderResultMap.entrySet()) {
          String testName = testResult.getKey().getName();
          String testDir = createTestDirectory(basePath, testName);
          writeTestResultToDisk(testDir, testResult.getValue());
        }
      } catch (Exception exception) {
        Timber.e(exception);
      }
    }
  }

  private File createTestResultRootFolder() {
    File testResultDir = new File(Environment.getExternalStorageDirectory() + "/mapbox");
    if (testResultDir.exists()) {
      // cleanup old files
      deleteRecursive(testResultDir);
    }

    if (!testResultDir.mkdirs()) {
      throw new RuntimeException("can't create root test directory");
    }
    return testResultDir;
  }

  private void deleteRecursive(File fileOrDirectory) {
    if (fileOrDirectory.isDirectory()) {
      for (File child : fileOrDirectory.listFiles()) {
        deleteRecursive(child);
      }
    }

    if (!fileOrDirectory.delete()) {
      throw new RuntimeException("can't delete directory");
    }
  }

  private String createTestDirectory(String basePath, String testName) {
    File testDir = new File(basePath + "/" + testName);
    if (!testDir.exists()) {
      if (!testDir.mkdir()) {
        throw new RuntimeException("can't create sub directory for " + testName);
      }
    }
    return testDir.getAbsolutePath();
  }

  private void writeTestResultToDisk(String testPath, Bitmap testResult) {
    try {
      String filePath = testPath + "/actual.png";
      FileOutputStream out = new FileOutputStream(filePath);
      testResult.compress(Bitmap.CompressFormat.PNG, 100, out);
      out.flush();
      out.close();
    } catch (Exception exception) {
      throw new RuntimeException(exception);
    }
  }

  @Override
  protected void onStop() {
    super.onStop();
    for (MapSnapshotter snapshotter : mapSnapshotterList) {
      snapshotter.cancel();
    }
  }

  private boolean isExternalStorageWritable() {
    return Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState());
  }

  public boolean isFinishedRendering() {
    return finishedRendering;
  }
}
