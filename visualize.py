import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from sklearn.cluster import KMeans
from sklearn.preprocessing import StandardScaler
import os
import imageio.v3 as iio # Required for GIF creation

# Path configuration
ORIGIN_CSV_PATH = 'origin.csv'
FINAL_CSV_PATH = 'results/political_coordinates-10000.csv' # Final state at step 10000
ANIMATION_FRAMES_DIR = 'animation_frames' # Directory for animation frames

# Ensure output directories exist
os.makedirs('results', exist_ok=True)
os.makedirs(ANIMATION_FRAMES_DIR, exist_ok=True)


print("1. Comparing Initial and Final (T=10000) States")

print(f"Loading data from {ORIGIN_CSV_PATH}...")
try:
    df_origin = pd.read_csv(ORIGIN_CSV_PATH, header=None)
    print(f"Origin data shape: {df_origin.shape}")
except FileNotFoundError:
    print(f"Error: {ORIGIN_CSV_PATH} not found. Ensure 'origin.csv' is in the same directory.")
    exit()

print(f"Loading data from {FINAL_CSV_PATH}...")
try:
    df_final = pd.read_csv(FINAL_CSV_PATH, header=None)
    print(f"Final data shape: {df_final.shape}")
except FileNotFoundError:
    print(f"Error: {FINAL_CSV_PATH} not found. Ensure 'results/political_coordinates-10000.csv' exists.")
    exit()

# Flatten 2D grid to 1D for distribution analysis
political_coords_origin = df_origin.values.flatten()
political_coords_final = df_final.values.flatten()

# Reshape for K-Means
political_coords_origin_2d = political_coords_origin.reshape(-1, 1)
political_coords_final_2d = political_coords_final.reshape(-1, 1)

# Calculate the difference map: Final - Initial
df_difference = df_final - df_origin

print("\nInitial State Statistics")
print(f"Mean: {np.mean(political_coords_origin):.4f}")
print(f"Variance: {np.var(political_coords_origin):.4f}")
print(f"Std Dev: {np.std(political_coords_origin):.4f}")
print(f"Min: {np.min(political_coords_origin):.4f}")
print(f"Max: {np.max(political_coords_origin):.4f}")

print("\nFinal State (T=10000) Statistics")
print(f"Mean: {np.mean(political_coords_final):.4f}")
print(f"Variance: {np.var(political_coords_final):.4f}")
print(f"Std Dev: {np.std(political_coords_final):.4f}")
print(f"Min: {np.min(political_coords_final):.4f}")
print(f"Max: {np.max(political_coords_final):.4f}")

print("\nChange (Final - Initial) Statistics")
print(f"Mean Change: {np.mean(df_difference.values.flatten()):.4f}")
print(f"Abs Max Change: {np.max(np.abs(df_difference.values.flatten())):.4f}")


# Visualization: Distribution Plots
plt.figure(figsize=(16, 7))

# Histogram with KDE
plt.subplot(1, 2, 1)
sns.histplot(political_coords_origin, bins=30, kde=True, color='skyblue', label='Initial State', stat='density')
sns.histplot(political_coords_final, bins=30, kde=True, color='salmon', label='Final State (T=10000)', alpha=0.7, stat='density')
plt.title('Distribution of Political Coordinates (Initial vs. Final)')
plt.xlabel('Political Coordinate')
plt.ylabel('Density')
plt.legend()
plt.grid(axis='y', linestyle='--', alpha=0.7)

# Box plot for summary comparison
plt.subplot(1, 2, 2)
data_for_box_plot = pd.DataFrame({
    'State': ['Initial'] * len(political_coords_origin) + ['Final (T=10000)'] * len(political_coords_final),
    'Political Coordinate': np.concatenate([political_coords_origin, political_coords_final])
})
sns.boxplot(x='State', y='Political Coordinate', data=data_for_box_plot, palette={'Initial': 'skyblue', 'Final (T=10000)': 'salmon'})
plt.title('Summary of Political Coordinates (Initial vs. Final)')
plt.ylabel('Political Coordinate')
plt.grid(axis='y', linestyle='--', alpha=0.7)

plt.tight_layout()
plt.savefig('results/distribution_comparison_initial_vs_10000.png')
plt.show()
plt.close()


# Visualization: Heatmaps
plt.figure(figsize=(20, 7))

# Initial State Heatmap
plt.subplot(1, 3, 1)
sns.heatmap(df_origin, cmap='RdBu', vmin=0, vmax=1, cbar_kws={'label': 'Political Coordinate'})
plt.title('Initial Political Landscape')
plt.xlabel('Column')
plt.ylabel('Row')
plt.gca().set_aspect('equal', adjustable='box')

# Final State Heatmap
plt.subplot(1, 3, 2)
sns.heatmap(df_final, cmap='RdBu', vmin=0, vmax=1, cbar_kws={'label': 'Political Coordinate'})
plt.title('Final Political Landscape (T=10000)')
plt.xlabel('Column')
plt.ylabel('Row')
plt.gca().set_aspect('equal', adjustable='box')

# Difference Heatmap (Final - Initial)
plt.subplot(1, 3, 3)
max_abs_diff = np.max(np.abs(df_difference.values))
sns.heatmap(df_difference, cmap='coolwarm', center=0, vmin=-max_abs_diff, vmax=max_abs_diff,
            cbar_kws={'label': 'Change in Political Coordinate (Final - Initial)'})
plt.title('Change in Political Landscape (Final - Initial)')
plt.xlabel('Column')
plt.ylabel('Row')
plt.gca().set_aspect('equal', adjustable='box')

plt.tight_layout()
plt.savefig('results/heatmap_comparison_initial_vs_10000.png')
plt.show()
plt.close()


# K-Means Clustering (2 Clusters)
print("\nK-Means Clustering Example (2 Clusters)")

# Scale data before clustering
scaler = StandardScaler()
scaled_origin_data = scaler.fit_transform(political_coords_origin_2d)
scaled_final_data = scaler.fit_transform(political_coords_final_2d)

# Cluster initial state
kmeans_origin = KMeans(n_clusters=2, random_state=42, n_init=10)
kmeans_origin.fit(scaled_origin_data)
origin_cluster_labels = kmeans_origin.labels_
origin_cluster_centers = kmeans_origin.cluster_centers_

print("\nInitial State Clusters:")
print(f"Cluster 0 Count: {np.sum(origin_cluster_labels == 0)}")
print(f"Cluster 1 Count: {np.sum(origin_cluster_labels == 1)}")
print(f"Cluster Centers (scaled): {origin_cluster_centers.flatten()}")

# Cluster final state
kmeans_final = KMeans(n_clusters=2, random_state=42, n_init=10)
kmeans_final.fit(scaled_final_data)
final_cluster_labels = kmeans_final.labels_
final_cluster_centers = kmeans_final.cluster_centers_

print("\nFinal State Clusters:")
print(f"Cluster 0 Count: {np.sum(final_cluster_labels == 0)}")
print(f"Cluster 1 Count: {np.sum(final_cluster_labels == 1)}")
print(f"Cluster Centers (scaled): {final_cluster_centers.flatten()}")

# Visualize clusters
plt.figure(figsize=(12, 5))

plt.subplot(1, 2, 1)
sns.scatterplot(x=political_coords_origin, y=np.zeros_like(political_coords_origin), hue=origin_cluster_labels, palette='viridis', legend='full', s=10)
plt.title('Initial State Clusters (Jittered for Visibility)')
plt.xlabel('Political Coordinate')
plt.yticks([])
plt.grid(axis='x', linestyle='--', alpha=0.7)

plt.subplot(1, 2, 2)
sns.scatterplot(x=political_coords_final, y=np.zeros_like(political_coords_final), hue=final_cluster_labels, palette='viridis', legend='full', s=10)
plt.title('Final State Clusters (Jittered for Visibility)')
plt.xlabel('Political Coordinate')
plt.yticks([])
plt.grid(axis='x', linestyle='--', alpha=0.7)

plt.tight_layout()
plt.savefig('results/cluster_comparison_initial_vs_10000.png')
plt.show()
plt.close()


print("\n2. Generating Animation Frames (60fps)")
# Generates heatmap images for each time step from 1 to 10000

image_filenames = [] # List to store filenames of generated images

for i in range(1, 10001): # Loop through all time steps
    frame_csv_path = os.path.join('results', f'political_coordinates-{i}.csv')
    try:
        df_frame = pd.read_csv(frame_csv_path, header=None)
        
        plt.figure(figsize=(8, 7)) # Set figure size
        sns.heatmap(df_frame, cmap='RdBu', vmin=0, vmax=1, cbar_kws={'label': 'Political Coordinate'})
        plt.title(f'Political Landscape at Time Step {i:04d}') # Format step number
        plt.xlabel('Column')
        plt.ylabel('Row')
        plt.gca().set_aspect('equal', adjustable='box') # Square aspect ratio
        plt.tight_layout()

        frame_filename = os.path.join(ANIMATION_FRAMES_DIR, f'political_landscape_frame_{i:04d}.png')
        plt.savefig(frame_filename)
        plt.close() # Close plot
        image_filenames.append(frame_filename)
        
        if i % 100 == 0: # Print progress
            print(f"Generated frame: {frame_filename}")

    except FileNotFoundError:
        print(f"Warning: {frame_csv_path} not found. Skipping frame {i}. Ensure all political_coordinates-N.csv files are in the 'results' directory.")
    except Exception as e:
        print(f"Error processing frame {i}: {e}")

print(f"\n{len(image_filenames)} animation frames saved to '{ANIMATION_FRAMES_DIR}'.")

# How to Create GIF Locally
print("\nCreating GIF Animation Locally")
print("Use the saved images to create a GIF.")
print("Method 1: Using Python's 'imageio' library (pip install imageio required)")
print(f"import imageio.v3 as iio")
print(f"import os")
print(f"image_filenames_sorted = sorted([os.path.join('{ANIMATION_FRAMES_DIR}', f) for f in os.listdir('{ANIMATION_FRAMES_DIR}') if f.endswith('.png')])")
print(f"frames_for_gif = [iio.imread(f) for f in image_filenames_sorted]")
print(f"iio.imwrite('political_landscape_animation.gif', frames_for_gif, fps=60)")
print("\nMethod 2: Using FFmpeg (external program)")
print(f"Run in terminal:")
print(f"ffmpeg -framerate 60 -i {ANIMATION_FRAMES_DIR}/political_landscape_frame_%04d.png -c:v libx264 -pix_fmt yuv420p political_landscape_animation.mp4")
print(f"ffmpeg -framerate 60 -i {ANIMATION_FRAMES_DIR}/political_landscape_frame_%04d.png political_landscape_animation.gif")
print("\n*Note*: All political_coordinates-N.csv files must be in the 'results' folder for animation generation.")
