package com.mapbox.mapboxsdk.testapp.utils;

import android.support.test.espresso.IdlingResource;

import com.mapbox.mapboxsdk.testapp.activity.render.RenderTestActivity;

public class SnapshotterIdlingResource implements IdlingResource {

  private final RenderTestActivity activity;
  private IdlingResource.ResourceCallback resourceCallback;

  public SnapshotterIdlingResource(RenderTestActivity activity) {
    this.activity = activity;
  }

  @Override
  public String getName() {
    return getClass().getSimpleName();
  }

  @Override
  public boolean isIdleNow() {
    boolean idle = isSnapshotReady();
    if (idle && resourceCallback != null) {
      resourceCallback.onTransitionToIdle();
    }
    return idle;
  }

  @Override
  public void registerIdleTransitionCallback(ResourceCallback resourceCallback) {
    this.resourceCallback = resourceCallback;
  }

  private boolean isSnapshotReady() {
    return activity != null && activity.isFinishedRendering();
  }
}
