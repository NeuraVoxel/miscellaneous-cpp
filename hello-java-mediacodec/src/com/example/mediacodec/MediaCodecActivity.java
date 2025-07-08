package com.example.mediacodec;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.media.MediaCodec;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.Surface;
import android.view.TextureView;
import android.widget.Button;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import java.nio.ByteBuffer;
import java.util.Arrays;

/**
 * 示例Activity，展示如何在Android应用中使用MediaCodecHelper
 * 
 * 注意：此Activity需要相机权限，并使用Camera2 API获取相机预览帧
 */
public class MediaCodecActivity extends AppCompatActivity {
    private static final String TAG = "MediaCodecActivity";
    
    // 请求相机权限的请求码
    private static final int REQUEST_CAMERA_PERMISSION = 200;
    
    // 视频参数
    private static final int VIDEO_WIDTH = 1280;
    private static final int VIDEO_HEIGHT = 720;
    
    // UI组件
    private TextureView mTextureView;
    private Button mStartButton;
    private Button mStopButton;
    
    // 相机相关
    private CameraDevice mCameraDevice;
    private CameraCaptureSession mCaptureSession;
    private HandlerThread mBackgroundThread;
    private Handler mBackgroundHandler;
    
    // 编解码相关
    private MediaCodecExample mMediaCodecExample;
    private boolean mIsEncoding = false;
    
    // 相机设备状态回调
    private final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(@NonNull CameraDevice camera) {
            mCameraDevice = camera;
            createCameraPreviewSession();
        }
        
        @Override
        public void onDisconnected(@NonNull CameraDevice camera) {
            camera.close();
            mCameraDevice = null;
        }
        
        @Override
        public void onError(@NonNull CameraDevice camera, int error) {
            camera.close();
            mCameraDevice = null;
            finish();
        }
    };
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_media_codec);
        
        // 初始化UI组件
        mTextureView = findViewById(R.id.texture_view);
        mStartButton = findViewById(R.id.start_button);
        mStopButton = findViewById(R.id.stop_button);
        
        // 设置TextureView监听器
        mTextureView.setSurfaceTextureListener(new TextureView.SurfaceTextureListener() {
            @Override
            public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
                openCamera();
            }
            
            @Override
            public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
            }
            
            @Override
            public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
                return true;
            }
            
            @Override
            public void onSurfaceTextureUpdated(SurfaceTexture surface) {
            }
        });
        
        // 设置按钮点击监听器
        mStartButton.setOnClickListener(v -> startEncoding());
        mStopButton.setOnClickListener(v -> stopEncoding());
        
        // 初始化MediaCodecExample
        mMediaCodecExample = new MediaCodecExample();
        
        // 设置编码回调
        mMediaCodecExample.setEncodedFrameCallback(new MediaCodecExample.EncodedFrameCallback() {
            @Override
            public void onEncodedFrame(ByteBuffer encodedData, MediaCodec.BufferInfo bufferInfo) {
                // 处理编码后的H.265数据
                // 在实际应用中，这里可能会将数据保存到文件或通过网络发送
                Log.d(TAG, "收到编码帧: " + bufferInfo.size + " bytes");
                
                // 解码并显示（在实际应用中，这可能在另一个设备或组件中进行）
                mMediaCodecExample.decodeFrame(encodedData, bufferInfo);
            }
        });
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        startBackgroundThread();
        
        if (mTextureView.isAvailable()) {
            openCamera();
        }
    }
    
    @Override
    protected void onPause() {
        stopEncoding();
        closeCamera();
        stopBackgroundThread();
        super.onPause();
    }
    
    /**
     * 启动后台线程
     */
    private void startBackgroundThread() {
        mBackgroundThread = new HandlerThread("CameraBackground");
        mBackgroundThread.start();
        mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
    }
    
    /**
     * 停止后台线程
     */
    private void stopBackgroundThread() {
        if (mBackgroundThread != null) {
            mBackgroundThread.quitSafely();
            try {
                mBackgroundThread.join();
                mBackgroundThread = null;
                mBackgroundHandler = null;
            } catch (InterruptedException e) {
                Log.e(TAG, "停止后台线程时出错: " + e.getMessage());
            }
        }
    }
    
    /**
     * 打开相机
     */
    private void openCamera() {
        CameraManager manager = (CameraManager) getSystemService(CAMERA_SERVICE);
        try {
            // 检查相机权限
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, REQUEST_CAMERA_PERMISSION);
                return;
            }
            
            // 获取后置相机ID
            String cameraId = null;
            for (String id : manager.getCameraIdList()) {
                CameraCharacteristics characteristics = manager.getCameraCharacteristics(id);
                Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);
                if (facing != null && facing == CameraCharacteristics.LENS_FACING_BACK) {
                    cameraId = id;
                    break;
                }
            }
            
            if (cameraId == null) {
                Log.e(TAG, "找不到后置相机");
                return;
            }
            
            // 打开相机
            manager.openCamera(cameraId, mStateCallback, mBackgroundHandler);
        } catch (CameraAccessException e) {
            Log.e(TAG, "打开相机时出错: " + e.getMessage());
        }
    }
    
    /**
     * 创建相机预览会话
     */
    private void createCameraPreviewSession() {
        try {
            SurfaceTexture texture = mTextureView.getSurfaceTexture();
            texture.setDefaultBufferSize(VIDEO_WIDTH, VIDEO_HEIGHT);
            
            Surface previewSurface = new Surface(texture);
            
            // 创建预览请求
            final CaptureRequest.Builder previewRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            previewRequestBuilder.addTarget(previewSurface);
            
            // 创建捕获会话
            mCameraDevice.createCaptureSession(Arrays.asList(previewSurface),
                new CameraCaptureSession.StateCallback() {
                    @Override
                    public void onConfigured(@NonNull CameraCaptureSession session) {
                        if (mCameraDevice == null) {
                            return;
                        }
                        
                        mCaptureSession = session;
                        try {
                            // 自动对焦
                            previewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
                            
                            // 开始预览
                            CaptureRequest previewRequest = previewRequestBuilder.build();
                            mCaptureSession.setRepeatingRequest(previewRequest, null, mBackgroundHandler);
                        } catch (CameraAccessException e) {
                            Log.e(TAG, "设置相机预览时出错: " + e.getMessage());
                        }
                    }
                    
                    @Override
                    public void onConfigureFailed(@NonNull CameraCaptureSession session) {
                        Toast.makeText(MediaCodecActivity.this, "相机配置失败", Toast.LENGTH_SHORT).show();
                    }
                }, null);
        } catch (CameraAccessException e) {
            Log.e(TAG, "创建相机预览会话时出错: " + e.getMessage());
        }
    }
    
    /**
     * 关闭相机
     */
    private void closeCamera() {
        if (mCaptureSession != null) {
            mCaptureSession.close();
            mCaptureSession = null;
        }
        
        if (mCameraDevice != null) {
            mCameraDevice.close();
            mCameraDevice = null;
        }
    }
    
    /**
     * 开始编码
     */
    private void startEncoding() {
        if (mIsEncoding) {
            return;
        }
        
        // 初始化编码器
        if (mMediaCodecExample.initEncoder()) {
            // 初始化解码器（使用TextureView的Surface进行渲染）
            SurfaceTexture texture = mTextureView.getSurfaceTexture();
            Surface decodeSurface = new Surface(texture);
            
            if (mMediaCodecExample.initDecoder(decodeSurface)) {
                mIsEncoding = true;
                Toast.makeText(this, "开始编码", Toast.LENGTH_SHORT).show();
                
                // 在实际应用中，这里会从相机获取YUV数据并进行编码
                // 这里仅作为示例，创建一个假的YUV帧
                byte[] fakeYuvFrame = new byte[VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2]; // YUV420格式
                
                // 编码帧（在实际应用中，这会在相机帧回调中进行）
                mMediaCodecExample.encodeFrame(fakeYuvFrame, System.nanoTime() / 1000);
            } else {
                Toast.makeText(this, "初始化解码器失败", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "初始化编码器失败", Toast.LENGTH_SHORT).show();
        }
    }
    
    /**
     * 停止编码
     */
    private void stopEncoding() {
        if (!mIsEncoding) {
            return;
        }
        
        mIsEncoding = false;
        mMediaCodecExample.release();
        Toast.makeText(this, "停止编码", Toast.LENGTH_SHORT).show();
    }
    
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_CAMERA_PERMISSION) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                openCamera();
            } else {
                Toast.makeText(this, "需要相机权限", Toast.LENGTH_SHORT).show();
                finish();
            }
        }
    }
}